const mssql = require('mssql');


let prop = {
    user: 'UniversityUser', password: '1111', server: 'WIN-UCLB12VI625', port:14625, database: 'UNIVER',
    pool: {max: 10, min: 4},
    options: {
        trustServerCertificate: true,
        enableArithAbort: true
    },
    connectionTimeout: 450000
};

function DB(callBack) {
        connect = new mssql.ConnectionPool(prop).connect()
            .then(pool => {
                console.log('connect to database');
                return pool;
            })
            .catch(err => 
                console.error('connection failed:', err.message)           
            );

     this.connect = mssql.connect(prop, err => {
        err ? callBack(err, null) : callBack(null, this.connect);
    });

    this.getFaculties = (args, context) => {
        return (new mssql.Request())
            .query('select * from faculty')
            .then(record => { console.log(record.recordset); return record.recordset });
    };

    this.getPulpits = (args, context) => {
        return (new mssql.Request())
            .query('select * from pulpit')
            .then(record => { return record.recordset; });
    };

    this.getSubjects = (args, context) => {
        return (new mssql.Request())
            .query('select * from subject')
            .then(record => { return record.recordset; });
    };

    this.getTeachers = (args, context) => {
        return (new mssql.Request())
            .query('select * from teacher')
            .then(record => { return record.recordset; });
    };

    this.getFaculty = (args, context) => {
        return (new mssql.Request())
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .query('select top(1) * from faculty where faculty = @faculty')
            .then(record => { console.log(record.recordset); return record.recordset; });
    };

    this.getPulpit = (args, context) => {
        return (new mssql.Request())
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .query('select top(1) * from pulpit where pulpit = @pulpit')
            .then(record => { return record.recordset; });
    };

    this.getSubject = (args, context) => {
        return (new mssql.Request())
            .input('subject', mssql.NVarChar, args.SUBJECT)
            .query('select top(1) * from subject where subject = @subject')
            .then(record => { return record.recordset; });
    };

    this.getTeacher = (args, context) => {
        return (new mssql.Request())
            .input('teacher', mssql.NVarChar, args.TEACHER)
            .query('select top(1) * from teacher where teacher = @teacher')
            .then(record => { return record.recordset; });
    };


    this.getTeachersByFaculty = (args, context) => {
        console.log(args);
        return (new mssql.Request())
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .query('select teacher.*, pulpit.faculty from teacher ' +
                'join pulpit on teacher.pulpit = pulpit.pulpit ' +
                'join faculty on pulpit.faculty = faculty.faculty where faculty.faculty = @faculty')
            .then(record => {
                let zaps = o => {
                    return {
                        TEACHER: o.TEACHER,
                        TEACHER_NAME: o.TEACHER_NAME,
                        PULPIT: o.PULPIT
                    }
                };
                let zapp = o => {
                    return {
                        FACULTY: o.FACULTY,
                        TEACHERS: [zaps(o)]
                    }
                };
                let rc = [];
                record.recordset.forEach((el, index) => {
                    if (index === 0)
                        rc.push(zapp(el));
                    else if (rc[rc.length - 1].FACULTY !== el.FACULTY)
                        rc.push(zapp(el));
                    else
                        rc[rc.length - 1].TEACHERS.push(zaps(el));
                });
                console.log(rc)
                return rc;
            })
    };


    this.getSubjectsByFaculties = (args, context) => {
        return (new mssql.Request())
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .query('select subject.*, pulpit.pulpit_name, pulpit.faculty from subject ' +
                'join pulpit on subject.pulpit = pulpit.pulpit ' +
                'join faculty on pulpit.faculty = faculty.faculty where faculty.faculty = @faculty')
            .then(record => {
                let zaps = o => {
                    return {
                        SUBJECT: o.SUBJECT,
                        SUBJECT_NAME: o.SUBJECT_NAME,
                        PULPIT: o.PULPIT
                    }
                };
                let zapp = o => {
                    return {
                        PULPIT: o.PULPIT,
                        PULPIT_NAME: o.PULPIT_NAME,
                        FACULTY: o.FACULTY,
                        SUBJECTS: [zaps(o)]
                    }
                };
                let rc = [];
                record.recordset.forEach((el, index) => {
                    if (index === 0)
                        rc.push(zapp(el));
                    else if (rc[rc.length - 1].PULPIT !== el.PULPIT)
                        rc.push(zapp(el));
                    else
                        rc[rc.length - 1].SUBJECTS.push(zaps(el));
                });
                console.log(rc)
                return rc;
            });
    };

    //----------------------------------------------------

    this.insertFaculty = (args, context) => {
        return (new mssql.Request())
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .input('faculty_name', mssql.NVarChar, args.FACULTY_NAME)
            .query('insert faculty(faculty, faculty_name) values (@faculty, @faculty_name)')
            .then(record => { return args });
    };

    this.insertPulpit = (args, context) => {
        return (new mssql.Request())
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .input('pulpit_name', mssql.NVarChar, args.PULPIT_NAME)
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .query('insert pulpit(pulpit, pulpit_name, faculty) values (@pulpit, @pulpit_name, @faculty)')
            .then(record => { return args });
    };

    this.insertSubject = (args, context) => {
        return (new mssql.Request())
            .input('subject', mssql.NVarChar, args.SUBJECT)
            .input('subject_name', mssql.NVarChar, args.SUBJECT_NAME)
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .query('insert subject(subject, subject_name, pulpit) values (@subject, @subject_name, @pulpit)')
            .then(record => { return args });
    };

    this.insertTeacher = (args, context) => {
        return (new mssql.Request())
            .input('teacher', mssql.NVarChar, args.TEACHER)
            .input('teacher_name', mssql.NVarChar, args.TEACHER_NAME)
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .query('insert teacher(teacher, teacher_name, pulpit) values (@teacher, @teacher_name, @pulpit)')
            .then(record => { return args });
    };


    //----------------------------------------------------

    this.updateFaculty = (args, context) => {
        return (new mssql.Request())
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .input('faculty_name', mssql.NVarChar, args.FACULTY_NAME)
            .query('update faculty set faculty = @faculty, faculty_name = @faculty_name where faculty = @faculty')
            .then(record => { return (record.rowsAffected[0] === 0) ? null : args; });
    };

    this.updatePulpit = (args, context) => {
        return (new mssql.Request())
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .input('pulpit_name', mssql.NVarChar, args.PULPIT_NAME)
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .query('update pulpit set pulpit = @pulpit, pulpit_name = @pulpit_name, faculty = @faculty where pulpit = @pulpit')
            .then(record => { return (record.rowsAffected[0] === 0) ? null : args; });
    };

    this.updateSubject = (args, context) => {
        return (new mssql.Request())
            .input('subject', mssql.NVarChar, args.SUBJECT)
            .input('subject_name', mssql.NVarChar, args.SUBJECT_NAME)
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .query('update subject set subject = @subject, subject_name = @subject_name, pulpit = @pulpit where subject = @subject')
            .then(record => { return (record.rowsAffected[0] === 0) ? null : args; });
    };

    this.updateTeacher = (args, context) => {
        return (new mssql.Request())
            .input('teacher', mssql.NVarChar, args.TEACHER)
            .input('teacher_name', mssql.NVarChar, args.TEACHER_NAME)
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .query('update teacher set teacher = @teacher, teacher_name = @teacher_name, pulpit = @pulpit where teacher = @teacher')
            .then(record => { return (record.rowsAffected[0] === 0) ? null : args; });
    };

    //----------------------------------------------------

    this.delFaculty = (args, context) => {
        return (new mssql.Request())
            .input('faculty', mssql.NVarChar, args.FACULTY)
            .query('delete from faculty where faculty = @faculty')
            .then(record => {
                return record.rowsAffected[0] == 0 ? null : args;
            });
    };

    this.delPulpit = (args, context) => {
        return (new mssql.Request())
            .input('pulpit', mssql.NVarChar, args.PULPIT)
            .query('delete from pulpit where pulpit = @pulpit')
            .then(record => {
                return record.rowsAffected[0] == 0 ? null : args;
            });
    };

    this.delSubject = (args, context) => {

        return (new mssql.Request())
            .input('subject', mssql.NVarChar, args.SUBJECT)
            .query('delete from subject where subject = @subject')
            .then(record => {
                return (record.rowsAffected[0] === 0) ? null : args;
            });
    };

    this.delTeacher = (args, context) => {
        return (new mssql.Request())
            .input('teacher', mssql.NVarChar, args.TEACHER)
            .query('delete from teacher where teacher = @teacher')
            .then(record => {
                return record.rowsAffected[0] == 0 ? null : args;
            });
    };
}




exports.DB = callBack => new DB(callBack);