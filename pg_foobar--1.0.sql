/* pg_foobar/pg_foobar--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_foobar" to load this file. \quit

-- pg_foobar returns always NULL
CREATE FUNCTION pg_foobar(
IN nworkers regclass,
IN foo_count int,
IN bar_count int)
RETURNS text
AS 'MODULE_PATHNAME', 'pg_foobar'
LANGUAGE C STRICT;
