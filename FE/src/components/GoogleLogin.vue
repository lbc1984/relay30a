<template>
    <button @click="handleLogin">Đăng nhập với Google</button>
  </template>
  
  <script>
  import { redirectToGoogleLogin } from "../auth";
  
  export default {
    methods: {
      handleLogin() {
        redirectToGoogleLogin(); // Chuyển hướng đến trang đăng nhập Google
      },
    },
  };
  </script>
  