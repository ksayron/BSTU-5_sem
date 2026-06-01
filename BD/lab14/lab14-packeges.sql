CREATE OR REPLACE PACKAGE pkg_game_job_core IS
  PROCEDURE perform_job(p_min_rating IN NUMBER := 5.0);
END pkg_game_job_core;
/

CREATE OR REPLACE PACKAGE BODY pkg_game_job_core IS

  PROCEDURE log_attempt_autonomous(
    p_status        IN NVARCHAR2,
    p_error_message IN NVARCHAR2,
    p_rows_moved    IN NUMBER
  ) IS
    PRAGMA AUTONOMOUS_TRANSACTION;
  BEGIN
    INSERT INTO job_status(status, error_message, rows_moved)
    VALUES (p_status, p_error_message, p_rows_moved);
    COMMIT;
  END;

  PROCEDURE perform_job(p_min_rating IN NUMBER := 5.0) IS
    v_rows NUMBER := 0;
  BEGIN
    INSERT INTO TARGET(name, rating)
    SELECT name, rating
    FROM SOURCE
    WHERE rating >= p_min_rating;

    v_rows := SQL%ROWCOUNT;

    DELETE FROM SOURCE
    WHERE rating >= p_min_rating;

    COMMIT;

    log_attempt_autonomous('SUCCESS', NULL, v_rows);
  EXCEPTION
    WHEN OTHERS THEN
      log_attempt_autonomous('FAILURE', SQLERRM, NULL);
      RAISE;
  END perform_job;

END pkg_game_job_core;
/

CREATE OR REPLACE PACKAGE pkg_game_job_dbmsjob IS
  PROCEDURE create_weekly_job;
  PROCEDURE run_now(p_job_id IN NUMBER);
  PROCEDURE stop_job(p_job_id IN NUMBER);      -- BROKEN = TRUE
  PROCEDURE resume_job(p_job_id IN NUMBER);    -- BROKEN = FALSE
  PROCEDURE remove_job(p_job_id IN NUMBER);

  FUNCTION is_running(p_job_id IN NUMBER) RETURN BOOLEAN;
END pkg_game_job_dbmsjob;
/

CREATE OR REPLACE PACKAGE BODY pkg_game_job_dbmsjob IS

  PROCEDURE create_weekly_job IS
    v_job NUMBER;
  BEGIN
    DBMS_JOB.SUBMIT(
      job       => v_job,
      what      => 'BEGIN pkg_game_job_core.perform_job(5.0); END;',
      next_date => SYSDATE,
      interval  => 'SYSDATE + 7'
    );
    COMMIT;
  END;

  PROCEDURE run_now(p_job_id IN NUMBER) IS
  BEGIN
    DBMS_JOB.RUN(p_job_id);
  END;

  PROCEDURE stop_job(p_job_id IN NUMBER) IS
  BEGIN
    DBMS_JOB.BROKEN(p_job_id, TRUE);
    COMMIT;
  END;

  PROCEDURE resume_job(p_job_id IN NUMBER) IS
  BEGIN
    DBMS_JOB.BROKEN(p_job_id, FALSE);
    COMMIT;
  END;

  PROCEDURE remove_job(p_job_id IN NUMBER) IS
  BEGIN
    DBMS_JOB.REMOVE(p_job_id);
    COMMIT;
  END;

  -- ✅ НОВОЕ: проверка выполнения
  FUNCTION is_running(p_job_id IN NUMBER) RETURN BOOLEAN IS
    v_cnt NUMBER;
  BEGIN
    SELECT COUNT(*)
    INTO v_cnt
    FROM USER_JOBS
    WHERE broken = 'N';

    RETURN v_cnt > 0;
  END;

END pkg_game_job_dbmsjob;
/

CREATE OR REPLACE PACKAGE pkg_game_job_sched IS
  PROCEDURE create_weekly_job;
  PROCEDURE run_now;
  PROCEDURE suspend_job;
  PROCEDURE resume_job;
  PROCEDURE drop_job;

  FUNCTION is_running RETURN BOOLEAN;
END pkg_game_job_sched;
/

CREATE OR REPLACE PACKAGE BODY pkg_game_job_sched IS

  PROCEDURE create_weekly_job IS
  BEGIN
    DBMS_SCHEDULER.CREATE_JOB(
      job_name        => 'GAME_JOB_SCHED',
      job_type        => 'STORED_PROCEDURE',
      job_action      => 'PKG_GAME_JOB_CORE.PERFORM_JOB',
      start_date      => SYSTIMESTAMP,
      repeat_interval => 'FREQ=WEEKLY',
      enabled         => TRUE
    );
  END;

  PROCEDURE run_now IS
  BEGIN
    DBMS_SCHEDULER.RUN_JOB('GAME_JOB_SCHED');
  END;

  PROCEDURE suspend_job IS
  BEGIN
    DBMS_SCHEDULER.DISABLE('GAME_JOB_SCHED');
  END;

  PROCEDURE resume_job IS
  BEGIN
    DBMS_SCHEDULER.ENABLE('GAME_JOB_SCHED');
  END;

  PROCEDURE drop_job IS
  BEGIN
    DBMS_SCHEDULER.DROP_JOB('GAME_JOB_SCHED');
  END;

  FUNCTION is_running RETURN BOOLEAN IS
    v_cnt NUMBER;
  BEGIN
    SELECT COUNT(*)
    INTO v_cnt
    FROM user_scheduler_jobs WHERE
    ENABLED='TRUE' and JOB_NAME='GAME_JOB_SCHED';

    RETURN v_cnt > 0;
  END;

END pkg_game_job_sched;
/