const http = require('http');
const { graphql, buildSchema } = require('graphql');
const { handleResponse, handleError } = require('./responce_handler');
const { DB } = require('./db_module');
const schema = buildSchema(require('fs').readFileSync('./schema.graphql').toString());
const resolver = require('./resolver');
const server = http.createServer();


const http_handler = (req, res) => {
    if (req.method === 'POST') {
        let reqData = '';
        req.on('data', chunk => { reqData += chunk; });
        req.on('end', () => {

            try {
                let json = JSON.parse(reqData);
                let variables = json.variables ? json.variables : {};
                console.log('\nquery:\n', json);

                if (json.query) {
                //console.log(schema);
                    graphql(schema, json.query, resolver, context, variables)
                        .then(result => {
                            if (result.errors) {
                                let json = JSON.stringify({ errorMessage: result.errors[0].message }, null, 4);
                                handleError(res, '\nerror Query:\n', json);
                            }
                            else if (result.data) {
                                let json = JSON.stringify(result.data, null, 4);
                                handleResponse(res, '\result:\n', json);
                            }
                        })
                }

                else if (json.mutation) {
                    graphql(schema, json.mutation, resolver, context, variables)
                        .then(result => {
                            if (result.errors) {
                                let json = JSON.stringify({ errorMessage: result.errors[0].message }, null, 4);
                                handleError(res, '\nerror Mutation:\n', json);
                            }
                            else if (result.data) {
                                let json = JSON.stringify(result.data, null, 4);
                                handleResponse(res, '\nresult:\n', json);
                            }
                        })
                }

                else {
                    handleError(res, '\nerror\n', JSON.stringify({ errorMessage: 'Invalid JSON request. Enter query or mutation' }));
                }
            }

            catch (err) {
                handleError(res, '\nerror\n', JSON.stringify({ errorMessage: `Request error: ${err.message}` }));
            }
        })
    }

    else {
        handleError(res, '\nerror\n', JSON.stringify({ errorMessage: 'Incorrect method' }));
    }
}




const context = DB(err => {
    if (err)
        console.error('error Cannot connect to database.',err);
    else {
        console.log('\nSuccesfully connected to database.');
        server.listen(5000, () => { console.log('Server running at localhost:5000/') })
            .on('error', err => { console.log('error ', err.code); })
            .on('request', http_handler);
    }
});