const resolver = {


    getFaculties: (args, context) => (args.FACULTY) ? context.getFaculty(args, context) : context.getFaculties(args, context),

    getPulpits: (args, context) => (args.PULPIT) ? context.getPulpit(args, context) : context.getPulpits(args, context),

    getSubjects: (args, context) => (args.SUBJECT) ? context.getSubject(args, context) : context.getSubjects(args, context),

    getTeachers: (args, context) => (args.TEACHER) ? context.getTeacher(args, context) : context.getTeachers(args, context),

    getTeachersByFaculty: (args, context) => context.getTeachersByFaculty(args, context),

    getSubjectsByFaculties: (args, context) => context.getSubjectsByFaculties(args, context),

    //-----------------------------------------------

    setFaculty: async (args, context) => {
        let res = await context.updateFaculty(args, context);
        return (res == null) ? context.insertFaculty(args, context) : res;
    },

    setPulpit: async (args, context) => {
        let res = await context.updatePulpit(args, context);
        return (res == null) ? context.insertPulpit(args, context) : res;
    },

    setSubject: async (args, context) => {
        let res = await context.updateSubject(args, context);
        return (res == null) ? context.insertSubject(args, context) : res;
    },

    setTeacher: async (args, context) => {
        let res = await context.updateTeacher(args, context);
        return (res == null) ? context.insertTeacher(args, context) : res;
    },

    //-------------------------------------------

    delFaculty: async (args, context) => {
        let deletedFaculty = (await context.getFaculty(args, context))[0];
        let res = await context.delFaculty(args, context);
        return (res == null) ? res : deletedFaculty;
    },

    delPulpit: (args, context) => { return context.delPulpit(args, context) },

    delSubject: (args, context) => context.delSubject(args, context),

    delTeacher: (args, context) => context.delTeacher(args, context)

};

module.exports = resolver;