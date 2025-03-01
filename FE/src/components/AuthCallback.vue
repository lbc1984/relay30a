<template>
    <div>Đang xác thực...</div>
  </template>
  
  <script>
  import { useRouter } from "vue-router";
  
  export default {
    name: "AuthCallback",
    mounted() {
      const router = useRouter();
      const hash = window.location.hash.substring(1); // Lấy chuỗi sau dấu `#`
      const params = new URLSearchParams(hash);
      const accessToken = params.get("access_token");
  
      if (accessToken) {
        localStorage.setItem("google_access_token", accessToken);
        router.push("/"); // Quay về trang chính
      } else {
        console.error("Không lấy được token!");
        router.push("/login"); // Quay lại trang login nếu thất bại
      }
    },
  };
  </script>
  