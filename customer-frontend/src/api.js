import axios from 'axios';

const api = axios.create({ baseURL: 'http://localhost:8082/api' });

// Attach token from localStorage on every request
api.interceptors.request.use(config => {
  const token = localStorage.getItem('customer_token');
  if (token) config.headers['X-Token'] = token;
  return config;
});

export const login   = (customerId, password) => api.post('/auth/login', { customerId, password });
export const getMe   = ()                      => api.get('/me');
export const getStats = ()                     => api.get('/me/stats');
export const getOrders = ()                    => api.get('/me/orders');
export const getLoyalty = ()                   => api.get('/me/loyalty');
export const redeemPoints = (points)           => api.post('/me/redeem', { points });
