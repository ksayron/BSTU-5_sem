-- Очистка таблиц
TRUNCATE TABLE SOURCE;
TRUNCATE TABLE TARGET;
TRUNCATE TABLE JOB_STATUS;


INSERT INTO SOURCE (name, rating) VALUES ('диско элизиум', 9.20);
INSERT INTO SOURCE (name, rating) VALUES ('клэш рояль', 5.60);
INSERT INTO SOURCE (name, rating) VALUES ('элден ринг', 1.90);
INSERT INTO SOURCE (name, rating) VALUES ('мгс 5', 9.60);
INSERT INTO SOURCE (name, rating) VALUES ('холлоу найт', 3.40);
INSERT INTO SOURCE (name, rating) VALUES ('6пи джамп', 4.70);
INSERT INTO SOURCE (name, rating) VALUES ('призрак цусимы', 8.70);
INSERT INTO SOURCE (name, rating) VALUES ('танчики с кмс', 10.00);

COMMIT;

SELECT * FROM SOURCE;
SELECT * FROM TARGET;
-- creation
BEGIN
  pkg_game_job_dbmsjob.create_weekly_job;
END;
/
set serveroutput on;
-- Проверка созданного задания
SELECT job, what, broken, next_date
FROM user_jobs;
INSERT INTO SOURCE (name, rating) VALUES ('клэш рояль 2', 6.60);
-- forced
DECLARE
  v_job NUMBER;
BEGIN
  SELECT job INTO v_job FROM user_jobs FETCH FIRST 1 ROWS ONLY;
  pkg_game_job_dbmsjob.run_now(v_job);
END;
/
SELECT * FROM JOB_STATUS ORDER BY attempt_time DESC;

-- stopping
DECLARE
  v_job NUMBER;
BEGIN
  SELECT job INTO v_job FROM user_jobs FETCH FIRST 1 ROWS ONLY;
  pkg_game_job_dbmsjob.stop_job(v_job);
END;
/

DECLARE
  v_job NUMBER;
BEGIN
  SELECT job INTO v_job FROM user_jobs FETCH FIRST 1 ROWS ONLY;
  pkg_game_job_dbmsjob.resume_job(v_job);
END;
/
DECLARE
  v_job NUMBER;
BEGIN
  SELECT job INTO v_job FROM user_jobs FETCH FIRST 1 ROWS ONLY;
  IF pkg_game_job_dbmsjob.is_running(v_job) THEN
    DBMS_OUTPUT.PUT_LINE('DBMS_JOB is running');
  ELSE
    DBMS_OUTPUT.PUT_LINE('DBMS_JOB is NOT running');
  END IF;
END;
/


--delete job
SELECT job, broken FROM user_jobs where broken = 'N';
DECLARE
  v_job NUMBER;
BEGIN
  SELECT job INTO v_job FROM user_jobs FETCH FIRST 1 ROWS ONLY;
  pkg_game_job_dbmsjob.remove_job(v_job);
END;
/

SELECT * FROM user_jobs;
