import express from 'express';

import routes from './routes.mjs';
import bodyParser from 'body-parser';



const port = 80;
const app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
    extended: true,
}));

const server = app.listen(port, (error) => {
    if (error) return console.log(`Error: ${error}`);
    console.log(`Server listening on port ${server.address().port}`);
});

routes(app);







