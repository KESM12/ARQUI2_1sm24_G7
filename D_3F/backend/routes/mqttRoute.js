const { Router } = require("express");
const router = Router();

const mqttController = require("../controllers/mqttController");

router.post("/mqtt", mqttController.publicacionesMQTT);

module.exports = router;
