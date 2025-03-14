importScripts("https://www.gstatic.com/firebasejs/10.8.1/firebase-app-compat.js");
importScripts("https://www.gstatic.com/firebasejs/10.8.1/firebase-messaging-compat.js");

// Cấu hình Firebase
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

// Lắng nghe tin nhắn khi ứng dụng chạy nền (background)
messaging.onBackgroundMessage((payload) => {
  console.log("[firebase-messaging-sw.js] Nhận thông báo nền:", payload);

  const notificationTitle = payload.notification?.title || "Thông báo mới";
  const notificationOptions = {
    body: payload.notification?.body || "Bạn có một thông báo mới.",
    icon: "/icon.png",
    data: payload.data, // Thêm dữ liệu để xử lý khi nhấn vào thông báo
    actions: [
      { action: "open_url", title: "Mở ứng dụng" }
    ]
  };

  self.registration.showNotification(notificationTitle, notificationOptions);
});

// Xử lý khi người dùng nhấn vào thông báo
self.addEventListener("notificationclick", (event) => {
  console.log("[firebase-messaging-sw.js] Người dùng nhấn vào thông báo:", event.notification);

  event.notification.close(); // Đóng thông báo

  // Mở trang web khi nhấn vào thông báo
  event.waitUntil(
    clients.openWindow("https://yourwebsite.com") // Thay bằng URL của bạn
  );
});
