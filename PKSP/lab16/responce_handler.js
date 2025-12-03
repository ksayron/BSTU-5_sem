
const handleResponse = (res, statusMessage, resMessage) => {
    console.log(statusMessage, resMessage);
    res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(resMessage);
}


const handleError = (res, statusMessage, errorMessage) => {
    console.log(statusMessage, errorMessage);
    res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(errorMessage);
}


module.exports = { handleResponse, handleError }