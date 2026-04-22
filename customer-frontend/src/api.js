import axios from 'axios';

const api = axios.create({ baseURL: 'http://localhost:8081/api' });

export const adminLogin       = (username, password) => api.post('/admin/login', { username, password });

export const getProducts      = ()           => api.get('/products');
export const addProduct       = (data)       => api.post('/admin/product', data);
export const updateProduct    = (id, data)   => api.put(`/admin/product/${id}`, data);
export const deleteProduct    = (id)         => api.delete(`/admin/product/${id}`);

export const getOrders        = ()           => api.get('/orders');
export const getAnalytics     = ()           => api.get('/analytics');
export const getRankedCustomers = ()         => api.get('/customers/ranked');
