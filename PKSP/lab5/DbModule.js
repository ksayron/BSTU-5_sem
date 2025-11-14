var util = require('util');
var events = require('events');

var DbSet = [
    { id: 1, name: "Petr", bday: "15-11-2000" },
    { id: 2, name: "Anna", bday: "16-07-2001" },
    { id: 3, name: "Viktor", bday: "25-09-1998" }
];

var commitCount = 0;

function DB() {
    this.select = () => {
        return DbSet;
    };

    this.insert = (insertElement) => {
        let parsedId

        if (insertElement.id == null) {
            console.log("ID was null");
            parsedId = 1;
        }
        else {
            parsedId = parseInt(insertElement.id);
        }

        while (true) {
            if (DbSet.findIndex(item => item.id == parsedId) != -1) {
                parsedId = parsedId + 1;
            }
            else {
                break;
            }
        }
        let modifiedElement = { id: parsedId, name: insertElement.name, bday: insertElement.bday }
        DbSet.push(modifiedElement);
    };

    this.update = (updateElement) => {
        const index = DbSet.findIndex(item => item.id == updateElement.id);
        if (index === -1) {
            console.log("No element with this id was found");
            return false;
        }
        DbSet[index] = updateElement;
        return true;
    };

    this.delete = (id) => {
        console.log(`Parsed id: ${parseInt(id)}`);
        const index = DbSet.findIndex(el => el.id == id)
        if (index === -1) {
            console.log("No element with this id was found");
            return false;
        }
        const deletedRow = DbSet.splice(index, 1)[0];
        return deletedRow;
    }
    this.commit = ()=>{
        console.log("Commiting...");
        return ++commitCount;
    }
}

util.inherits(DB, events.EventEmitter);

exports.DB = DB;