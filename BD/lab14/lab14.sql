alter session set container=knp_pdb;
alter pluggable DATABASE knp_pdb open;
show con_name;

GRANT CREATE SESSION TO KNP;
GRANT CREATE TABLE TO KNP;
GRANT CREATE PROCEDURE TO KNP;
GRANT CREATE SEQUENCE TO KNP;
GRANT CREATE JOB TO KNP;
GRANT EXECUTE ON DBMS_SCHEDULER TO KNP;

-- Пару моментов:
-- В лабе не сказано, в каком коннекте делать и прочее, поэтому:
-- 1) Используем коннект, в котором работали в 8 лабе (база)

alter session set nls_date_format='dd-mm-yyyy hh24:mi:ss';
------------------------ Пакет DBMS_JOB ------------------------

-- 1. Разработайте пакет выполнения заданий, в котором:
-- Раз в неделю в определенное время выполняется копирование части данных 
-- по условию из одной таблицы в другую, 
-- после чего эти данные из первой таблицы удаляются.

drop table SOURCE;
drop table TARGET;
drop table job_status;

CREATE TABLE SOURCE
(
  id     NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  name   NVARCHAR2(200),
  rating NUMBER(4,2)
);

CREATE TABLE TARGET
(
  id     NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  name   NVARCHAR2(200),
  rating NUMBER(4,2)
);

-- Таблица для логирования попыток выполнения заданий
CREATE TABLE job_status
(
  attempt_id     NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  job_name       NVARCHAR2(100),
  attempt_time   TIMESTAMP DEFAULT SYSTIMESTAMP,
  status         NVARCHAR2(30), -- 'SUCCESS'|'FAILURE'|'START'
  error_message  NVARCHAR2(1000),
  rows_moved     NUMBER
);


