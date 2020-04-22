import AWS from 'aws-sdk';
import fs from 'fs';

// AWS.config.update({
//   "region": "us-east-1",
//   "accessKeyId": "ASIA3NXQMWU74CZWDXK7",
//   "secretAccessKey": "Fc8Av9GEEkJoRl6Fq5Y7jFJcnHoRASxY9LSZU7/y",
//   "sessionToken": "FwoGZXIvYXdzEPH//////////wEaDF7dH1HdDq9YSKnvQSLNAeORAQswC97Yu2dg+2Ye5iKuH7s72UBVdTd3nh2K1UDtJ8XCIeUcwmpGEtICfCPOYGs2LNCWV3ip+uGuiQOmku7Jv1TY/EdBUYSLnRWksRMryQaSOh0lsxZ6ZD9shCVMvy/MIG/FchdBSWtHEkyDw0Gbvxd4kVceV+22Q1c9HtR5N7mD3uD8kC7367dBwyqm1VCxOZgr11zHlFjtojEMwzF1bI3RoPsY6actI21bPGIX4TQENl8JhJHg6DzybFHsiUNNEQa4arXy9/yQkH8o8sHz9AUyLQcOUK/tMV+O6x8roOfBCeLIqWube2BboOtkSn19uPcOIWbuSEa6d0N1TpcS/A=="
// });

AWS.config.update({
  region: "us-east-1"
});
let credentials = new AWS.SharedIniFileCredentials({profile: 'default'});

AWS.config.credentials = credentials;

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
        message: err.message
      });
    } else if(data.Count == 1) {
      console.log("Login successfully: " + device_id);
      res.statusCode = 200;
      res.json({
        message: device_id
      });
    } else if(data.Count == 0) {
      res.json({
        message: "Device's id or password is incorrect"
      });
    }
  });
}

function getSensorData(req, res) {
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

function sendSMSWarning(req, res) {
    var params = {
        Message: "FireAlarm's WARNING !!! Smoke is detected in your house now !!!", //req.query.message,
        PhoneNumber: '+' + req.body.number,
        MessageAttributes: {
            'AWS.SNS.SMS.SenderID': {
                'DataType': 'String',
                'StringValue': "GasWarning"
            }
        }
    };

    var publishTextPromise = new AWS.SNS({ apiVersion: '2010-03-31' }).publish(params).promise();

    publishTextPromise.then(
        function (data) {
            res.end(JSON.stringify({ MessageID: data.MessageId }));
        }).catch(
            function (err) {
                res.end(JSON.stringify({ Error: err }));
            });

}

const Service = {
  login, getSensorData, sendSMSWarning
}

export default Service;
export { login, getSensorData, sendSMSWarning }



