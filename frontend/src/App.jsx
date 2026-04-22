import { BrowserRouter, Routes, Route } from 'react-router-dom';
import { Toaster } from 'react-hot-toast';
import Navbar from './components/Navbar';
import Catalog from './pages/Catalog';
import CartPage from './pages/CartPage';
import Checkout from './pages/Checkout';
import Orders from './pages/Orders';
import Admin from './pages/Admin';
import Analytics from './pages/Analytics';

export default function App() {
  return (
    <BrowserRouter>
      <Toaster
        position="top-right"
        toastOptions={{
          style: {
            background: '#1a1a24',
            color: '#e8e8f0',
            border: '1px solid rgba(212,175,55,0.2)',
            fontFamily: 'Inter, sans-serif',
          },
          success: { iconTheme: { primary: '#d4af37', secondary: '#0a0a0f' } },
        }}
      />
      <Navbar />
      <main style={{ paddingTop: '70px', minHeight: '100vh' }}>
        <Routes>
          <Route path="/"          element={<Catalog />} />
          <Route path="/cart"      element={<CartPage />} />
          <Route path="/checkout"  element={<Checkout />} />
          <Route path="/orders"    element={<Orders />} />
          <Route path="/analytics" element={<Analytics />} />
          <Route path="/admin"     element={<Admin />} />
        </Routes>
      </main>
    </BrowserRouter>
  );
}
