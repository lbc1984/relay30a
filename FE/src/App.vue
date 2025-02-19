<template>
  <v-app>
    <h1>Device Management</h1>

    <v-container>
      <v-row>
        <v-col cols="6" md="6" sm="12">
          <v-table>
            <tbody>
              <tr>
                <td>Trạng thái</td>
                <td>{{ this.server_status }}</td>
              </tr>
            </tbody>
          </v-table>
        </v-col>
      </v-row>
      <v-row>
        <v-col cols="12">
          <div class="devices">
            <div v-for="device in devices" :key="device.mac" class="device-card">
              <p><b>Name:</b> {{ device.name }}</p>
              <p><b>MAC:</b> {{ device.mac }}</p>
              <p><b>Status:</b> {{ device.status }}</p>
              <button @click="toggleDevice(device)">
                {{ device.status === 'on' ? 'Turn Off' : 'Turn On' }}
              </button>
            </div>
          </div>
        </v-col>
      </v-row>
    </v-container>
  </v-app>
</template>

<script>
import { ref, onMounted } from 'vue';
import mqtt from 'mqtt';

export default {
  name: 'DeviceManager',
  setup() {
    const devices = ref([
      {
        name: "name 1",
        mac: "mac 1",
        status: 'on'
      },
      {
        name: "name 2",
        mac: "mac2",
        status: 'on'
      }
    ]);
    let client = null;

    const connectToMQTT = () => {
      client = mqtt.connect('wss://c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud:8884/mqtt', {
        username: 'lybaocuong',
        password: '1234@Abcde',
      });

      client.on('connect', () => {
        console.log('Connected to MQTT');
        this.server_status = "Connected with server"
        client.subscribe('devices/+/status');
      });

      client.on('message', (topic, message) => {
        handleMessage(topic, message.toString());
      });
    };

    const handleMessage = (topic, message) => {
      const macAddress = topic.split('/')[1];
      const status = JSON.parse(message).status;

      const index = devices.value.findIndex((d) => d.mac === macAddress);
      if (index >= 0) {
        devices.value[index].status = status;
      } else {
        devices.value.push({ mac: macAddress, status });
      }
    };

    const toggleDevice = (device) => {
      const newStatus = device.status === 'on' ? 'off' : 'on';
      client.publish(`devices/${device.mac}/control`, JSON.stringify({ status: newStatus }));
    };

    onMounted(() => {
      connectToMQTT();
    });

    return {
      devices,
      toggleDevice,
    };
  },
  data() {
    return {
      server_status: "offline"
    }
  }
};
</script>

<style scoped>
.devices {
  display: flex;
}

.device-card {
  border: 1px solid #ccc;
  padding: 10px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  width: 250px;
  margin: 10px;
}

button {
  background-color: #4CAF50;
  color: white;
  padding: 5px 10px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background-color: #45a049;
}
</style>
