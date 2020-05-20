import AWS from 'aws-sdk';
import fs from 'fs';

AWS.config.update({
  "region": "us-east-1",
  "accessKeyId": "ASIA3NXQMWU76ONBKZ2H",
  "secretAccessKey": "ZRVPHJ5BUs695yshSuDz32Cp/gCVjRReuLhLd5eI",
  "sessionToken": "FwoGZXIvYXdzEMP//////////wEaDDzK6cXjiyF3gQr6kCLNAaxRPLTCCVRhHBxDugWjqRpHkhYC43UA92uybhECHF+5UNSVmpSG5xk5sltskYkDy1d4yYSS/NFGMOVk0g7C5V0LUQReOkip8WuFkEKRSOQe9/8qEI8PvkWIGJkfzH72axcpWmqFQDWXC3UKVNSuuWycdkt93SStRm3ZXM6VH9rigW2JeVi7CsZrxkkIPdDF6UIYRIHz1ipkHWMfZ10BGyWT3GQcipSf9M2OaOhufMde1jTX04A8IbvlZjvNGG6M1B1vCz2+jOJIzbwuzMoooZ2S9gUyLeb7I1IOSjDLoBTWQ4r54MXhtzY9mXZTe9ey0tLvrLpi9Zw3uvRBOBP8Iq9Esg=="
});

// AWS.config.update({
//   region: "us-east-1"
// });
// let credentials = new AWS.SharedIniFileCredentials({ profile: 'default' });

// AWS.config.credentials = credentials;

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
    } else if (data.Count == 1) {
      console.log("Login successfully: " + device_id);
      console.log(data);
      let fcm_token_sv = data.Items[0].fcm_token;
      if (fcm_token_sv === '' || fcm_token_sv != req.body.fcm_token) {
        let deviceFcmToken = req.body.fcm_token;

        //create an ARN endpoint in AWS SNS
        //application FireAlarm
        var sns = new AWS.SNS({ apiVersion: '2010-03-31' });
        let paramsCreateEP = {
          PlatformApplicationArn : "arn:aws:sns:us-east-1:785409422655:app/GCM/FireAlarm",
          Token : deviceFcmToken,
          CustomUserData : device_id,
          Attributes: {}
        }
        let endpointArn = "";
        sns.createPlatformEndpoint(paramsCreateEP, function(err, data) {
          if (err) {
            console.error("Unable to create an endpoint. Error", JSON.stringify(err, null, 2));
          } else {
            endpointArn = data.EndpointArn;
            console.log("CreateEndpoint succeeded:", JSON.stringify(data, null, 2));
          }
        });
        
        //wait to create Endpoint ARN

        // update fcm token in table device in DynamoDb
        var paramsUpdate = {
          TableName: table_device,
          Key:{
              "device_id": device_id
          },
          UpdateExpression: "set fcm_token = :f", //, target_arn = :t",
          ExpressionAttributeValues:{
              ":f": deviceFcmToken//,
              // FIXME:
              //":t": endpointArn
          },
          ReturnValues:"UPDATED_NEW"
        };
      
        console.log("Updating the item...");
        docClient.update(paramsUpdate, function(err, data) {
            if (err) {
                console.error("Unable to update item. Error JSON:", JSON.stringify(err, null, 2));
            } else {
                console.log("UpdateItem succeeded:", JSON.stringify(data, null, 2));
            }
        });
        console.log("EndpointARN: " + endpointArn);
      }
      res.statusCode = 200;
      res.json({
        message: device_id
      });
    } else if (data.Count == 0) {
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
  console.log(`WARNING!!! ${req.body.deviceId} with value ${req.body.warningValue} to number ${req.body.warningNumber}, type ${req.body.warningType} !`);
  
  var params = {
    Message: "FireAlarm's WARNING !!! Smoke is detected in your house now !!!", //req.query.message,
    PhoneNumber: '+' + req.body.warningNumber,
    MessageAttributes: {
      'AWS.SNS.SMS.SenderID': {
        'DataType': 'String',
        'StringValue': "SmokeWarning"
      }
    }
  };

  var publishTextPromise = new AWS.SNS({ apiVersion: '2010-03-31' }).publish(params).promise();

  var paramsPushNotif = {
    Message: "FireAlarm's WARNING !!! Smoke is detected in your house now !!!", /* required */
    Subject: "SmokeWarning",
    TargetArn: 'TOPIC_ARN'
  }

  var publishNotifPromise = new AWS.SNS({ apiVersion: '2010-03-31' }).publish(params).promise();

  publishNotifPromise.then(
    function(data) {
      console.log("publish notification succeed: "+ data);
    }
  ).catch(
    function(err) {
      console.error("publish notification failed: " + err);
    }
  );

  publishTextPromise.then(
    function (data) {
      res.end(JSON.stringify({ MessageID: data.MessageId }));
      console.log(`WARNING_SUCCESSFUL!!! ${req.body.deviceId} with value ${req.body.warningValue} to number ${req.body.warningNumber} !`);
    }
  ).catch(
    function (err) {
        res.status(400).end(JSON.stringify({ Error: err }));
        console.log(`WARNING_FAILED!!! ${req.body.deviceId} with value ${req.body.warningValue} to number ${req.body.warningNumber} !
        with failed error ${err.message}`);
    }
  );

}

function saveDeviceSetting(req, res) {

}


function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}   


const Service = {
  login, getSensorData, sendSMSWarning, saveDeviceSetting
}

export default Service;
export { login, getSensorData, sendSMSWarning, saveDeviceSetting }



