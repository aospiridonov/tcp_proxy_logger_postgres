#!/bin/bash
sleep 5;
for t in $(seq 1 100) 
do
  PGPASSWORD=${DB_PASSWORD:-root} psql --host=${DB_HOST:-localhost} \
       --port=${DB_PORT:-5432}\
       --username=${DB_USERNAME:-admin} \
       --dbname=${DB_NAME:-postgres} \
       --command="DROP TABLE IF EXISTS sbtest${t}" > /dev/null 2>&1
done

sysbench --pgsql-host=${DB_HOST:-localhost} \
         --pgsql-port=${DB_PORT:-5432} \
         --pgsql-user=${DB_USERNAME:-admin} \
         --pgsql-password=${DB_PASSWORD:-root} \
         --pgsql-db=${DB_NAME:-postgres} \
         --pgsql-sslmode=disable \
         /usr/local/share/sysbench/oltp_read_write.lua prepare

sysbench --pgsql-host=${DB_HOST:-localhost} \
         --pgsql-port=${DB_PORT:-5432} \
         --pgsql-user=${DB_USERNAME:-admin} \
         --pgsql-password=${DB_PASSWORD:-password} \
         --pgsql-db=${DB_NAME:-postgres} \
         --pgsql-sslmode=disable \
         --threads=${SYSBENCH_THREADS:-1} \
         --time=${SYSBENCH_TIME:-60} \
         /usr/local/share/sysbench/oltp_read_write.lua run