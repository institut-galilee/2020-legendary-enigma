import fa_service from "./service.mjs"


function routes(app) {
  /* TODO: API
    Login
    Change setting - Bluetooth ?
    Test Alarm - Bluetooth ?
    Graph
  */
  app.post('/login', (request, response) => {
    fa_service.login(request, response);
  });

  app.post('/sensor_data', (request, response) => {
    fa_service.getSensorData(request, response);
  });

  app.post('/send_sms', (request, response) => {
    fa_service.sendSMSWarning(request, response);
  });
}

export default routes;