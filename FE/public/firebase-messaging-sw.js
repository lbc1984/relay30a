importScripts("https://www.gstatic.com/firebasejs/10.8.1/firebase-app-compat.js");
importScripts("https://www.gstatic.com/firebasejs/10.8.1/firebase-messaging-compat.js");

firebase.initializeApp({
  apiKey: "AIzaSyCXSu97M3_ONeEV5GL2bn9MheySzvjTAzQ",
  authDomain: "mqtt-d8e66.firebaseapp.com",
  projectId: "mqtt-d8e66",
  storageBucket: "mqtt-d8e66.firebasestorage.app",
  messagingSenderId: "324700447868",
  appId: "1:324700447868:web:9106ccd46a65a730ae95f9",
  measurementId: "G-R1EDFFDF0K"
});

const messaging = firebase.messaging();

// Lắng nghe tin nhắn khi ứng dụng chạy nền
messaging.onBackgroundMessage((payload) => {
  console.log("[firebase-messaging-sw.js] Nhận thông báo nền: ", payload);
  self.registration.showNotification(payload.notification.title, {
    body: payload.notification.body,
    icon: "/icon.png",
  });
});
