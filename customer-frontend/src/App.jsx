import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import { Toaster } from 'react-hot-toast';
import { AuthProvider, useAuth } from './AuthContext';
import Navbar from './components/Navbar';
import Login from './pages/Login';
import Analytics from './pages/Analytics';
import Inventory from './pages/Inventory';

function ProtectedRoute({ children }) {
  const { authed } = useAuth();
  return authed ? children : <Navigate to="/" replace />;
}

function AppRoutes() {
  const { authed } = useAuth();

  return (
    <>
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
      {authed && <Navbar />}
      <main style={{ paddingTop: authed ? '70px' : '0', minHeight: '100vh' }}>
        <Routes>
          <Route path="/" element={authed ? <Navigate to="/analytics" replace /> : <Login />} />
          <Route path="/analytics" element={<ProtectedRoute><Analytics /></ProtectedRoute>} />
          <Route path="/inventory"  element={<ProtectedRoute><Inventory /></ProtectedRoute>} />
          <Route path="*"           element={<Navigate to="/" replace />} />
        </Routes>
      </main>
    </>
  );
}

export default function App() {
  return (
    <BrowserRouter>
      <AuthProvider>
        <AppRoutes />
      </AuthProvider>
    </BrowserRouter>
  );
}
