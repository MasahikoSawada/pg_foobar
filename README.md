# pg_foobar
foo bar parallel programing on PostgreSQL

Supports 9.6, 10-beta1.

# Installation
```
$ git clone git@github.com:MasahikoSawada/pg_foobar.git
$ cd pg_foobar
$ make USE_PGXS=1
$ su
# make install USE_PGXS=1
```

# Set up PostgreSQL server
```
$ initdb -D $PGPATH -E UTF8 --no-locale
$ pg_ctl start
$ psql
=# CREATE EXTENSION pg_foobar;
=# ¥dx
                  List of installed extensions
   Name    | Version |   Schema   |         Description
-----------+---------+------------+------------------------------
 pg_foobar | 1.0     | public     | foo bar game
 plpgsql   | 1.0     | pg_catalog | PL/pgSQL procedural language
(2 rows)
```

# Usage
pg_foobar extension provides one function `pg_foobar(nworkers, foo_count, bar_count)` which launches `nworkers` parallel workers that emits 'foo' at `foo_count` times and emits 'bar' at `bar_count` times to PostgreSQL server log file.
For example,

```
=# SELECT pg_foobar(2, 3, 4);
 pg_foobar
-----------

(1 row)
$ cat /path/to/postgresql.conf
2016-12-09 16:17:44.482 JST [47495] LOG:  worker [1] foo 0
2016-12-09 16:17:44.482 JST [47495] LOG:  worker [1] foo 1
2016-12-09 16:17:44.482 JST [47496] LOG:  worker [0] foo 0
2016-12-09 16:17:44.482 JST [47495] LOG:  worker [1] foo 2
2016-12-09 16:17:44.482 JST [47495] LOG:  worker [1] bar 0
2016-12-09 16:17:44.482 JST [47496] LOG:  worker [0] foo 1
2016-12-09 16:17:44.482 JST [47495] LOG:  worker [1] bar 1
2016-12-09 16:17:44.482 JST [47496] LOG:  worker [0] foo 2
2016-12-09 16:17:44.482 JST [47495] LOG:  worker [1] bar 2
2016-12-09 16:17:44.482 JST [47496] LOG:  worker [0] bar 0
2016-12-09 16:17:44.482 JST [47495] LOG:  worker [1] bar 3
2016-12-09 16:17:44.482 JST [47496] LOG:  worker [0] bar 1
2016-12-09 16:17:44.482 JST [47496] LOG:  worker [0] bar 2
2016-12-09 16:17:44.482 JST [47496] LOG:  worker [0] bar 3
```
