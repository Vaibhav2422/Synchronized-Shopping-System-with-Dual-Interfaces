import { Link, useLocation, useNavigate } from 'react-router-dom';
import { BarChart2, Package, LogOut, Shield } from 'lucide-react';
import { useAuth } from '../AuthContext';

const links = [
  { to: '/analytics', label: 'Analytics', icon: BarChart2 },
  { to: '/inventory',  label: 'Inventory', icon: Package },
];

export default function Navbar() {
  const loc = useLocation();
  const navigate = useNavigate();
  const { signOut } = useAuth();

  const handleLogout = () => {
    signOut();
    navigate('/');
  };

  return (
    <nav style={{
      position: 'fixed', top: 0, left: 0, right: 0, zIndex: 100,
      background: 'rgba(10,10,15,0.92)', backdropFilter: 'blur(20px)',
      borderBottom: '1px solid rgba(212,175,55,0.12)',
      height: '64px', display: 'flex', alignItems: 'center',
      padding: '0 32px', gap: '8px',
    }}>
      {/* Logo */}
      <div style={{ marginRight: '24px', display: 'flex', alignItems: 'center', gap: '10px' }}>
        <div style={{
          width: 32, height: 32, borderRadius: '8px',
          background: 'linear-gradient(135deg, #d4af37, #b8960a)',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
        }}>
          <Shield size={18} color="#0a0a0f" />
        </div>
        <span style={{ fontFamily: 'Playfair Display, serif', fontSize: '18px', fontWeight: 700 }}
              className="gold-text">Admin Panel</span>
      </div>

      {/* Nav links */}
      <div style={{ display: 'flex', gap: '4px', flex: 1 }}>
        {links.map(({ to, label, icon: Icon }) => {
          const active = loc.pathname === to;
          return (
            <Link key={to} to={to} style={{ textDecoration: 'none' }}>
              <div style={{
                display: 'flex', alignItems: 'center', gap: '6px',
                padding: '7px 14px', borderRadius: '8px', fontSize: '13px', fontWeight: 500,
                color: active ? '#d4af37' : '#8888a0',
                background: active ? 'rgba(212,175,55,0.08)' : 'transparent',
                border: active ? '1px solid rgba(212,175,55,0.2)' : '1px solid transparent',
                transition: 'all 0.2s', cursor: 'pointer',
              }}>
                <Icon size={15} />
                {label}
              </div>
            </Link>
          );
        })}
      </div>

      {/* Logout */}
      <button className="btn btn-outline" style={{ padding: '6px 12px', fontSize: '12px' }} onClick={handleLogout}>
        <LogOut size={13} /> Logout
      </button>
    </nav>
  );
}
