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
-- creation
BEGIN
  pkg_game_job_sched.create_weekly_job;
END;
/

SELECT job_name, enabled, state
FROM user_scheduler_jobs;


set serveroutput on;
-- Проверка созданного задания
INSERT INTO SOURCE (name, rating) VALUES ('клэш рояль 2', 6.60);
-- forced
BEGIN
  pkg_game_job_sched.run_now;
END;
/

SELECT * FROM TARGET;
SELECT * FROM SOURCE;
SELECT * FROM JOB_STATUS ORDER BY attempt_time DESC;
--stopping
BEGIN
  pkg_game_job_sched.suspend_job;
END;
/

SELECT job_name, enabled
FROM user_scheduler_jobs where ENABLED='TRUE' and JOB_NAME='GAME_JOB_SCHED';

--resuming
BEGIN
  pkg_game_job_sched.resume_job;
END;
/

SELECT job_name, enabled
FROM user_scheduler_jobs;

--check if running

BEGIN
  IF pkg_game_job_sched.is_running THEN
    DBMS_OUTPUT.PUT_LINE('SCHEDULER JOB is running');
  ELSE
    DBMS_OUTPUT.PUT_LINE('SCHEDULER JOB is NOT running');
  END IF;
END;
/
--delete
BEGIN
  pkg_game_job_sched.drop_job;
END;
/
SELECT * FROM user_scheduler_jobs ;


