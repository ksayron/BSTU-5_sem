drop table FACULTY;
drop table PULPIT;
drop table TEACHER;
drop table SUBJECT;
create table FACULTY
(
    FACULTY      nvarchar(10) primary key, 
    FACULTY_NAME nvarchar(100)             
);
create table PULPIT
(
    PULPIT      nvarchar(10) primary key,
    PULPIT_NAME nvarchar(100),
    FACULTY     nvarchar(10) foreign key references FACULTY (FACULTY)
);
create table TEACHER
(
    TEACHER      nvarchar(10) primary key,
    TEACHER_NAME nvarchar(100),
    PULPIT       nvarchar(10) foreign key references PULPIT (PULPIT)
);
create table SUBJECT
(
    SUBJECT      nvarchar(10) primary key,
    SUBJECT_NAME nvarchar(100),
    PULPIT       nvarchar(10) foreign key references PULPIT (PULPIT)
);
INSERT INTO FACULTY (FACULTY, FACULTY_NAME) VALUES ('ИТ', 'Факультет информационных технологий'), ('ИЭФ', 'Инженерно-Экономический');
INSERT INTO PULPIT (PULPIT, PULPIT_NAME, FACULTY) VALUES ('ИСиТ', 'Информационные системы и технологии', 'ИТ'), ('ПИ', 'Программная инженерия', 'ИТ'), ('ФИН', 'Финансы', 'ИЭФ');
INSERT INTO TEACHER (TEACHER, TEACHER_NAME, PULPIT) VALUES ('СМВ', 'Смелов В.В.', 'ИСиТ'), ('ШМН', 'Шиман Д.В.', 'ПИ');
INSERT INTO SUBJECT (SUBJECT, SUBJECT_NAME, PULPIT) VALUES ('ПСКП', 'Программирование серверных кроссплатформенных приложений', 'ИСиТ'), ('ПСП', 'Программирование сетевых приложений', 'ПИ'), ('МАТ', 'Математический анализ', 'ФИН');
