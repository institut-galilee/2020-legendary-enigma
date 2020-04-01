export default function routes(app) {
  /* TODO: API
    Login
    Change setting - Bluetooth ?
    Test Alarm - Bluetooth ?
    Graph
  */
  app.post('/login', (request, response) => {
      response.send(
          { message: 'Login successful test' }
      );
  });

  
};