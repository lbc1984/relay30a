<template>
  <v-app>
    <router-view :key="$route.fullPath"></router-view>
  </v-app>
</template>

<script>
export default {
  name: "App",
};
</script>

<style src="@/assets/css/styles.css"></style>
