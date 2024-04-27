import { createApp } from "vue";
import "./style.css";
import App from "./App.vue";
import { library } from "@fortawesome/fontawesome-svg-core";
import { FontAwesomeIcon } from "@fortawesome/vue-fontawesome";

import {
  faTemperatureLow,
  faAngleDown,
  faAngleUp,
  faFan,
  faLightbulb,
  faWind,
  faPeopleArrows,
  faPowerOff,
  faDroplet,
  faTemperatureQuarter,
  faSave,
  faSync
} from "@fortawesome/free-solid-svg-icons";
import { faDev } from "@fortawesome/free-brands-svg-icons";

library.add(
  faDev,
  faTemperatureLow,
  faAngleDown,
  faAngleUp,
  faFan,
  faLightbulb,
  faWind,
  faPeopleArrows,
  faPowerOff,
  faDroplet,
  faTemperatureQuarter,
  faSave,
  faSync
);
const app = createApp(App);

app.component("font-awesome-icon", FontAwesomeIcon);
app.mount("#app");