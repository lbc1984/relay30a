<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <v-toolbar color="primary" dark>
          <v-toolbar-title class="text-center w-100">Quản lý thiết bị</v-toolbar-title>
        </v-toolbar>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="12" md="3" lg="3" sm="4">
        <v-table>
          <tbody>
            <tr>
              <td>Server status:</td>
              <td>{{ this.server_status }}</td>
            </tr>
          </tbody>
        </v-table>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="12">
        <v-divider>Danh sách thiết bị</v-divider>
      </v-col>
      <v-col cols="12" class="d-flex justify-center align-center" style="height: 50vh" v-if="is_loading">
        <div class="spinner_dashboard"></div>
      </v-col>

      <v-col cols="12" md="4" lg="3" sm="6" v-for="device in devices" :key="device.mac" v-if="!is_loading">
        <v-card variant="elevated" color="indigo">
          <template v-slot:title>
            {{ device.mac }}
          </template>

          <template v-slot:subtitle>
            <v-text-field density="compact" v-model="device.name" @blur="saveName(device)"
              @focus="name_old = device.name" label="Số phòng"></v-text-field>
          </template>

          <template v-slot:text>
            <p><b>Status:</b> {{ device.status }}</p>
          </template>

          <template v-slot:actions v-if="device.status != 'aoffline'">
            <div class="d-flex justify-space-between w-100">
              <v-btn class="mt-2" @click="toggleDevice(device)" variant="tonal">
                {{ device.switch === "1" ? "Turn On" : "Turn Off" }}
              </v-btn>
              <v-btn class="mt-2" variant="tonal" @click="openScheduleDialog(device)">
                Timer
              </v-btn>
            </div>
          </template>
        </v-card>
      </v-col>
    </v-row>

    <DialogTimer :device="device" :show="show_schedule" @close="closeScheduleDialog" />
  </v-container>
</template>

<style src="@/assets/css/dashboard.css"></style>

<script>
import mqtt from "mqtt";
import axios from "axios";
import { jwtDecode } from "jwt-decode";
import DialogTimer from "./DialogTimer.vue";

export default {
  name: "Dashboard",
  setup() { },
  data() {
    return {
      server_status: "Disconnected",
      devices: [],
      client: null,
      name_old: "",
      is_loading: true,
      device: null,
      show_schedule: false,
    };
  },
  components: {
    DialogTimer,
  },
  methods: {
    connectToMQTT(url, username, password) {
      let port = 8884;
      let url_mqtt = "wss://" + url + ":" + port + "/mqtt";

      this.client = mqtt.connect(url_mqtt, {
        username: username,
        password: password,
      });

      this.client.on("connect", () => {
        this.server_status = "Connected";
        this.client.subscribe("device/+/name");
        this.client.subscribe("device/+/status");
        this.client.subscribe("device/+/switch");
      });

      this.client.on("message", (topic, message) => {
        this.handleMessage(topic, message.toString());
      });
    },

    handleMessage(topic, message) {
      const topic_split = topic.split("/");
      const macAddress = topic_split[1];
      const type = topic_split[2];

      const item = this.devices.find((d) => d.mac === macAddress);
      if (item) {
        if (type == "status") item.status = message;
        else if (type == "switch") item.switch = message;
        else if (type == "name") item.name = message;
      } else {
        const device = {
          mac: macAddress,
          status: type == "status" ? message : "",
          switch: type == "switch" ? message : "",
          name: type == "name" ? message : "",
        };

        this.devices.push(device);
      }
    },

    toggleDevice(device) {
      let status_switch = device.switch == "1" ? "0" : "1";
      let item = this.devices.find((x) => x.mac == device.mac);
      item.switch = status_switch;

      this.client.publish(`device/${device.mac}/switch`, status_switch, {
        qos: 1,
        retain: true,
      });
    },
    saveName(device) {
      if (this.name_old != device.name)
        this.client.publish(`device/${device.mac}/name`, device.name, {
          qos: 1,
          retain: true,
        });
    },
    openScheduleDialog(device) {
      this.device = device;
      this.show_schedule = true;
    },
    closeScheduleDialog() {
      this.show_schedule = false;
      this.device = null;
    },
  },
  mounted() {
    const token = localStorage.getItem("google_access_token");
    if (!token) {
      this.$router.push("/login");
    }

    axios
      .get("https://mqtt.sieuthitiendung.com/mqtt", {
        headers: { Authorization: `Bearer ${token}` },
      })
      .then(async (response) => {
        const token = response.headers["x-data"];
        const data = jwtDecode(token);

        this.connectToMQTT(data.url, data.user, data.password);
        this.is_loading = false;
      })
      .catch((error) => {
        this.$router.push("/login");
      });
  },
};
</script>
