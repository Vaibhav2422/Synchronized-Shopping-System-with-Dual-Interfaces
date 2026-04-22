import axios from 'axios';

const api = axios.create({ baseURL: 'http://localhost:8081/api' });

export const getProducts    = ()           => api.get('/products');
export const searchProducts = (q)          => api.get(`/products/search?q=${encodeURIComponent(q)}`);
export const filterProducts = (cat)        => api.get(`/products/filter?category=${encodeURIComponent(cat)}`);

export const getCart        = ()           => api.get('/cart');
export const addToCart      = (productId, quantity = 1) => api.post('/cart/add', { productId, quantity });
export const removeFromCart = (productId)  => api.post('/cart/remove', { productId });
export const clearCart      = ()           => api.post('/cart/clear');

export const getCoupons     = ()           => api.get('/coupons');
export const applyCoupon    = (code)       => api.post('/coupon/apply', { code });
export const removeCoupon   = (code)       => api.post('/coupon/remove', { code });

export const getBill        = (redeem = false) => api.get(`/bill?redeem=${redeem}`);
export const splitBill      = (count)      => api.post('/bill/split', { count });

export const getLoyalty     = ()           => api.get('/loyalty');
export const getCustomerLoyalty = (token)  =>
  axios.get('http://localhost:8082/api/me/loyalty', { headers: { 'X-Token': token } });

export const checkout       = (data)       => api.post('/checkout', data);

export const getOrders      = ()           => api.get('/orders');
export const getAnalytics   = ()           => api.get('/analytics');

export const adminLogin     = (username, password) => api.post('/admin/login', { username, password });
export const addProduct     = (data)       => api.post('/admin/product', data);
export const updateProduct  = (id, data)   => api.put(`/admin/product/${id}`, data);
export const deleteProduct  = (id)         => api.delete(`/admin/product/${id}`);

export const searchCustomers  = (q)        => api.get(`/customers/search?q=${encodeURIComponent(q)}`);
export const registerCustomer = (data)     => api.post('/customers/register', data);
export const getRankedCustomers = ()       => api.get('/customers/ranked');
export const verifyCustomer   = (customerId, password) =>
  axios.post('http://localhost:8082/api/auth/login', { customerId, password });
