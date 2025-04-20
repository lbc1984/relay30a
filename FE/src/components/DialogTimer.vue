<template>
    <v-dialog v-model="dialog" max-width="500">
        <v-card>
            <v-card-title class="text-h6 py-2">Schedules</v-card-title>
            <v-card-text>
                <v-row v-for="(event, index) in scheduleForm" :key="index" class="mb-1" no-gutters>
                    <v-col cols="3" class="pr-1">
                        <v-select v-model="event.day" :items="daysOfWeek" label="Thứ" dense hide-details outlined
                            class="py-0" :error="scheduleErrors[index]?.day"
                            :error-messages="scheduleErrors[index]?.day ? 'Chọn thứ' : ''"
                            @update:modelValue="() => clearDayError(index)" />
                    </v-col>
                    <v-col cols="4" class="px-1">
                        <v-menu v-model="event.timeMenu" :close-on-content-click="false" transition="scale-transition"
                            offset-y>
                            <template v-slot:activator="{ props }">
                                <v-text-field v-model="event.time" label="Giờ" readonly dense hide-details outlined
                                    v-bind="props" :error="scheduleErrors[index]?.time"
                                    :error-messages="scheduleErrors[index]?.time ? 'Chọn giờ' : ''" />
                            </template>
                            <v-time-picker v-if="event.timeMenu" v-model="event.time" format="24hr" full-width
                                @update:modelValue="() => clearTimeError(index)" />
                        </v-menu>
                    </v-col>
                    <v-col cols="4" class="px-1">
                        <v-select v-model="event.action" :items="['ON', 'OFF']" label="Bật/Tắt" dense hide-details
                            outlined class="py-0" />
                    </v-col>
                    <v-col cols="1" class="d-flex align-center justify-end pl-0">
                        <v-btn icon size="x-small" color="red" @click="deleteEvent(index)">
                            <v-icon size="18">mdi-close</v-icon>
                        </v-btn>
                    </v-col>
                </v-row>
                <v-row>
                    <v-btn size="x-small" class="mt-2" color="blue-darken-1" @click="addEvent">
                        + schedule
                    </v-btn>
                </v-row>
            </v-card-text>
            <v-card-actions class="justify-end px-4 pb-3">
                <v-btn size="small" color="primary" @click="saveSchedule">Lưu</v-btn>
                <v-btn size="small" @click="cancelDialog">Hủy</v-btn>
            </v-card-actions>
        </v-card>
    </v-dialog>
    <v-snackbar v-model="snackbar" color="green" timeout="3000" location="top">
        {{ this.message }}
    </v-snackbar>
</template>


<script>
import { set, onValue, db, ref } from "../firebase";
import { cloneDeep } from 'lodash';

export default {
    name: "DialogTimer",
    props: {
        device: Object,
        show: Boolean
    },
    data() {
        return {
            dialog: false,
            selectedDevice: null,
            scheduleForm: [{ day: '', time: '', action: 'ON', timeMenu: false }],
            deviceSchedules: {},
            scheduleErrors: [],
            daysOfWeek: ['1', '2', '3', '4', '5', '6', '7'],
            snackbar: false,
            message: "",
        };
    },
    emits: ["close"],
    watch: {
        show(newVal) {
            if (newVal) {
                this.dialog = true;
                this.openScheduleDialog(this.device);
            }
            else this.dialog = false;
        },
        dialog(newVal) {
            if (!newVal) {
                this.$emit("close");
            }
        },
    }
    ,
    methods: {
        openScheduleDialog(device) {
            this.selectedDevice = device;
            const mac = device.mac.toLowerCase().replace(/:/g, "");

            const deviceSchedule = this.deviceSchedules[mac]?.events;

            if (deviceSchedule && Array.isArray(deviceSchedule)) {
                this.scheduleForm = cloneDeep(deviceSchedule);
            } else {
                this.scheduleForm = [{ time: "", action: "ON", timeMenu: false }];
            }

            this.sortSchedule();
            this.dialog = true;
        },
        addEvent() {
            this.scheduleForm.push({ time: "", action: "OFF", timeMenu: false });
            this.scheduleErrors.push(false);
        },
        deleteEvent(index) {
            this.scheduleForm.splice(index, 1);
            this.scheduleErrors.splice(index, 1);
        },
        async saveSchedule() {
            if (!this.selectedDevice) return;

            let hasError = false;
            this.scheduleErrors = this.scheduleForm.map((event) => {
                const error = {
                    day: !event.day || event.day.trim() === "",
                    time: !event.time || event.time.trim() === ""
                };
                if (error.day || error.time) hasError = true;

                return error;
            });

            if (hasError) {
                this.$nextTick(() => {
                    const el = document.querySelector(".v-input--error input");
                    if (el) el.scrollIntoView({ behavior: "smooth", block: "center" });
                });
                return;
            }

            const mac = this.selectedDevice.mac.toLowerCase().replace(/:/g, "");
            const scheduleRef = ref(db, 'schedule/' + mac);

            try {
                const cleanSchedule = this.scheduleForm.map(item => {
                    const { timeMenu, ...rest } = item;
                    return rest;
                });

                console.log(cleanSchedule);

                await set(scheduleRef, { events: cleanSchedule });
                if (!this.deviceSchedules[mac]) {
                    this.deviceSchedules[mac] = { events: [] };
                }

                this.deviceSchedules[mac].events = this.scheduleForm.map(item => ({
                    ...item,
                    timeMenu: false
                }));
                this.cancelDialog();
                this.showMessage("Lưu schedules thành công");
            } catch (err) {
                this.showMessage("Lỗi khi lưu schedules");
                console.error("Lỗi khi lưu schedule:", err);
            }
        },
        listenToSchedules() {
            const schedulesRef = ref(db, 'schedule');

            onValue(schedulesRef, (snapshot) => {
                if (!snapshot.exists()) return;

                const schedules = snapshot.val() || {};

                this.deviceSchedules = Object.fromEntries(
                    Object.entries(schedules).map(([deviceId, deviceData]) => [
                        deviceId,
                        {
                            events: (deviceData.events || []).map(item => ({
                                ...item,
                                timeMenu: false
                            }))
                        }
                    ])
                );

            });
        },
        cancelDialog() {
            this.dialog = false;
            this.scheduleErrors = [];
        },
        sortSchedule() {
            if (Array.isArray(this.scheduleForm)) {
                // Định nghĩa thứ tự các ngày trong tuần (dùng số và CN cho Chủ nhật)
                const dayOrder = ['1', '2', '3', '4', '5', '6', '7'];

                this.scheduleForm.sort((a, b) => {
                    // So sánh ngày trong tuần trước
                    const day1Index = dayOrder.indexOf(a.day);
                    const day2Index = dayOrder.indexOf(b.day);
                    if (day1Index !== day2Index) {
                        return day1Index - day2Index;
                    }

                    // Nếu ngày giống nhau, sắp xếp theo giờ
                    const [h1, m1] = a.time.split(':').map(Number);
                    const [h2, m2] = b.time.split(':').map(Number);
                    return h1 * 60 + m1 - (h2 * 60 + m2);
                });
            }
        },
        clearDayError(index) {
            if (this.scheduleErrors[index]) this.scheduleErrors[index].day = false;
        },
        clearTimeError(index) {
            if (this.scheduleErrors[index]) this.scheduleErrors[index].time = false;
        },
        showMessage(message) {
            this.snackbar = true;
            this.message = message;
            setTimeout(() => {
                this.err_firebase = "";
            }, 3000);
        }

    },
    mounted() {
        this.listenToSchedules();
    },

};
</script>