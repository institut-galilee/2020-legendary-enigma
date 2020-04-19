import AWS from 'aws-sdk';
import fs from 'fs';

AWS.config.update({
  "region": "us-east-1",
  "accessKeyId": "ASIA3NXQMWU7ZM5G3JZQ",
  "secretAccessKey": "Sed0Rl4hEjO/sL5rRckVxzgdhEqTieMnGy/Ag52N",
  "sessionToken": "FwoGZXIvYXdzEKH//////////wEaDI4RdwatSPsH6AJ8JiLNAQ4sbeRlE1uVbQnek979Gsa/iDg+UHhUHO/b5wP4Z1j3c/JmVBWra1/oe4OQ5duGCqnIRx9y/k2saIah4+E0vXWu2j6fpRQHn9AzoCNUYqaV7xAlmeg8v2OdU2Nt1mvw/fXBS84hJervuSWJv3dR61Z/JqOTeCNb6hebV8MmGY6uDFLho/FTZCoi/UgUr8uiwFrKVtcd/Z9a3NPSSlS0sZ/EbHrv1PT30NBLrOXvrOvt4CMnC+DTlgNoLtQKPy6ueoPB4xUKTsc+z2dYJykojvHh9AUyLZLtoXOvcgpcK24mi5lZlHyoJk+SvMK78tvIpS+5yjOuB/S9hiYYV0woh5RjBQ=="
});
const table_sensor = "sensor_data";
const table_device = "device";


var docClient = new AWS.DynamoDB.DocumentClient();
function login(req, res) {
  let device_id = req.body.device_id;
  let password = req.body.password;
  let params = {
    TableName: table_device,
    KeyConditionExpression: '#id = :id',
    FilterExpression: '#pw = :pw',
    ExpressionAttributeNames: {
      '#id': 'device_id',
      '#pw': 'password'
    },
    ExpressionAttributeValues: {
      ':id': device_id,
      ':pw': password
    }
  };

  docClient.query(params, function (err, data) {
    if (err) {
      console.error("Unable to read item. Error JSON", JSON.stringify(data, null, 2));
      console.error(err);
      res.statusCode = 400;
      res.json({
        message: "Device's id or password is incorrect"
      });
    } else if(data.Count == 1) {
      console.log("Login successfully: " + device_id);
      res.statusCode = 200;
      res.json("message: " + device_id);
    }
  });
}

function getSensorData(req, res, next) {
  let device_id = req.body.device_id;
  let from = req.body.from;
  let to = req.body.to;
  var params = {
    TableName: table_sensor,
    KeyConditionExpression: '#id = :id AND #ts BETWEEN :t1 AND :t2',
    ExpressionAttributeValues: {
      ':id': device_id,
      ':t1': from,
      ':t2': to 
    },
    ExpressionAttributeNames: {
      '#id': 'device_id',
      '#ts': 'timestamp'
    }
  }
  
  docClient.query(params, function (err, data) {
    if (err) {
      console.error("Unable to read item. Error JSON", JSON.stringify(data, null, 2));
      console.error(err);
    } else {
      console.log("Get Item Succeeded:", JSON.stringify(data, null, 2));
    }

    if (data) {
      res.json(data);
      console.log(data);
    } else {
      res.statusCode = 400;
      res.json({
        message: "There is an error"
      });
    }
  });
}

const Service = {
  login, getSensorData
}

export default Service;
export { login, getSensorData }



