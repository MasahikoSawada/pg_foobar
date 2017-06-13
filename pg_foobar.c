/*-------------------------------------------------------------------------
 *
 * pg_foobar
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/htup_details.h"
#include "access/heapam.h"
#include "access/parallel.h"
#include "access/relscan.h"
#include "access/xact.h"
#include "catalog/pg_type.h"
#include "catalog/storage_xlog.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "storage/bufmgr.h"
#include "storage/procarray.h"
#include "storage/smgr.h"
#include "storage/spin.h"
#include "utils/guc.h"
#include "utils/memutils.h"
#include "utils/rel.h"

#define FOO_KEY 1000
#define BAR_KEY 1001

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_foobar);

void foobar_worker(dsm_segment *seg, shm_toc *toc);

void
foobar_worker(dsm_segment *seg, shm_toc *toc)
{
	int n_foo;
	int n_bar;
	int *shm_area;
	int i;

	/* Look up for foo count */
#if PG_VERSION_NUM >= 100000
	shm_area = (int *) shm_toc_lookup(toc, FOO_KEY, true);
#else
	shm_area = (int *) shm_toc_lookup(toc, FOO_KEY);
#endif

	n_foo = *shm_area;

	/* Look up for foo count */
#if PG_VERSION_NUM >= 100000
	shm_area = (int *) shm_toc_lookup(toc, BAR_KEY, true);
#else
	shm_area = (int *) shm_toc_lookup(toc, BAR_KEY);
#endif
	n_bar = *shm_area;

	/* foo */
	for (i = 0; i < n_foo; i++)
		elog(LOG, "worker [%d] foo %d", ParallelWorkerNumber, i);

	/* bar */
	for (i = 0; i < n_bar; i++)
		elog(LOG, "worker [%d] bar %d", ParallelWorkerNumber, i);
}

Datum
pg_foobar(PG_FUNCTION_ARGS)
{
	int nworkers = PG_GETARG_INT32(0);
	int n_foo = PG_GETARG_INT32(1);
	int n_bar = PG_GETARG_INT32(2);
	ParallelContext *pcxt;

	int size = 0,
		keys = 0;
	int *shm_area;

	/* Begin parallel mode */
	EnterParallelMode();

#if PG_VERSION_NUM >= 100000
	pcxt = CreateParallelContext("pg_foobar", "foobar_worker", nworkers);
#else
	pcxt = CreateParallelContext(foobar_worker, nworkers);
#endif

	/* Estimate DSM size for storing two integer */
	size += BUFFERALIGN(sizeof(int));
	keys++;
	size += BUFFERALIGN(sizeof(int));
	keys++;
	shm_toc_estimate_chunk(&pcxt->estimator, size);
	shm_toc_estimate_keys(&pcxt->estimator, keys); /* foo count and bar cound */
	InitializeParallelDSM(pcxt);

	/* Set up DSM for foo */
	shm_area = (int *) shm_toc_allocate(pcxt->toc, sizeof(int));
	shm_toc_insert(pcxt->toc, FOO_KEY, shm_area);
	*shm_area = n_foo;

	/* Set up DSM for bar */
	shm_area = (int *) shm_toc_allocate(pcxt->toc, sizeof(int));
	shm_toc_insert(pcxt->toc, BAR_KEY, shm_area);
	*shm_area = n_bar;

	/* Do, foo bar */
	LaunchParallelWorkers(pcxt);

	/* Wait for parallel worker finish */
	WaitForParallelWorkersToFinish(pcxt);
	
	/* Finalize parallel scanning */
	DestroyParallelContext(pcxt);
	ExitParallelMode();
	
	PG_RETURN_NULL();
}
