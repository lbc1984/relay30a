import { createRouter, createWebHistory } from "vue-router";
import Dashboard from "../components/Dashboard.vue";
import AuthCallback from "../components/AuthCallback.vue";
import Login from "../components/Login.vue";
const routes = [
  { path: "/", component: Dashboard, meta: { requiresAuth: true } },
  { path: "/auth/callback", component: AuthCallback },
  {path: "/login", component: Login},
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

router.beforeEach((to, from, next) => {
  const token = localStorage.getItem("google_access_token");

  if (to.meta.requiresAuth && !token) {
    next("/login");
  } else {
    next();
  }
});

export default router;
