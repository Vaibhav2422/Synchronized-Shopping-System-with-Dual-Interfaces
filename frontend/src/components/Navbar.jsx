import { Link, useLocation } from 'react-router-dom';
import { ShoppingCart, Package, BarChart2, Shield, History, Store } from 'lucide-react';
import { useState, useEffect } from 'react';
import { getCart } from '../api';

const links = [
  { to: '/',          label: 'Catalog',    icon: Store },
  { to: '/cart',      label: 'Cart',       icon: ShoppingCart },
  { to: '/orders',    label: 'Orders',     icon: History },
  { to: '/analytics', label: 'Analytics',  icon: BarChart2 },
  { to: '/admin',     label: 'Admin',      icon: Shield },
];

export default function Navbar() {
  const loc = useLocation();
  const [cartCount, setCartCount] = useState(0);

  useEffect(() => {
    const refresh = () => getCart().then(r => setCartCount(r.data.itemCount || 0)).catch(() => {});
    refresh();
    const id = setInterval(refresh, 3000);
    return () => clearInterval(id);
  }, []);

  return (
    <nav style={{
      position: 'fixed', top: 0, left: 0, right: 0, zIndex: 100,
      background: 'rgba(10,10,15,0.92)', backdropFilter: 'blur(20px)',
      borderBottom: '1px solid rgba(212,175,55,0.12)',
      height: '64px', display: 'flex', alignItems: 'center',
      padding: '0 32px', gap: '8px',
    }}>
      {/* Logo */}
      <Link to="/" style={{ textDecoration: 'none', marginRight: '24px', display: 'flex', alignItems: 'center', gap: '10px' }}>
        <div style={{
          width: 32, height: 32, borderRadius: '8px',
          background: 'linear-gradient(135deg, #d4af37, #b8960a)',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
        }}>
          <Package size={18} color="#0a0a0f" />
        </div>
        <span style={{ fontFamily: 'Playfair Display, serif', fontSize: '18px', fontWeight: 700 }}
              className="gold-text">ShopMS</span>
      </Link>

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
                transition: 'all 0.2s', cursor: 'pointer', position: 'relative',
              }}>
                <Icon size={15} />
                {label}
                {label === 'Cart' && cartCount > 0 && (
                  <span style={{
                    position: 'absolute', top: -4, right: -4,
                    background: '#d4af37', color: '#0a0a0f',
                    borderRadius: '50%', width: 18, height: 18,
                    fontSize: 10, fontWeight: 700,
                    display: 'flex', alignItems: 'center', justifyContent: 'center',
                  }}>{cartCount}</span>
                )}
              </div>
            </Link>
          );
        })}
      </div>
    </nav>
  );
}
