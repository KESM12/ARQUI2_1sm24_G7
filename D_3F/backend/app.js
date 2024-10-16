const express = require("express");
const morgan = require("morgan");
const cors = require("cors");

const app = express();
const PORT = process.env.PORT || 5000;

//Middlewares
app.use(express.json())
app.use(morgan("dev"));
app.use(cors());

//Route Initial API
app.get("/", (req, res) => {
    res.json({ message: "ARQUI2 - Fase 3 - API MQTT" });
});

//Routes API
app.use('/api', require('./routes/mqttRoute'))


// MQTT Conexion
const mqttController = require('./controllers/mqttController')
const clientMQTT = mqttController.client

clientMQTT.on('connect', mqttController.suscribirse)

clientMQTT.on('message', mqttController.mensajesMQTT)

  
app.listen(PORT, () => {
    console.log(`API is listening on port ${PORT}`);
});