import { useEffect, useState } from 'react';
import { IndianRupee, TrendingDown, Package, Tag, Star, AlertCircle } from 'lucide-react';
import { getStats, getMe } from '../api';
import { useAuth } from '../AuthContext';

function StatCard({ icon: Icon, label, value, sub, color = 'var(--gold)' }) {
  return (
    <div className="glass" style={{ padding: '24px', display: 'flex', flexDirection: 'column', gap: '12px' }}>
      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
        <span style={{ fontSize: '12px', fontWeight: 600, color: 'var(--text-dim)', textTransform: 'uppercase', letterSpacing: '0.5px' }}>
          {label}
        </span>
        <div style={{
          width: 36, height: 36, borderRadius: '10px',
          background: `${color}18`,
          display: 'flex', alignItems: 'center', justifyContent: 'center',
        }}>
          <Icon size={18} color={color} />
        </div>
      </div>
      <div style={{ fontSize: '28px', fontWeight: 700, color: 'var(--text)' }}>{value}</div>
      {sub && <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>{sub}</div>}
    </div>
  );
}

export default function Dashboard() {
  const { customer, setCustomer } = useAuth();
  const [stats, setStats] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const load = async () => {
      try {
        const [statsRes, meRes] = await Promise.all([getStats(), getMe()]);
        setStats(statsRes.data);
        setCustomer(meRes.data);
      } catch (err) {
        setError(err.response?.data?.error || 'Failed to load dashboard');
      } finally {
        setLoading(false);
      }
    };
    load();
  }, [setCustomer]);

  if (loading) {
    return (
      <div style={{ padding: '40px 32px' }}>
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(220px, 1fr))', gap: '16px' }}>
          {[...Array(5)].map((_, i) => (
            <div key={i} className="skeleton" style={{ height: '120px' }} />
          ))}
        </div>
      </div>
    );
  }

  if (error) {
    return (
      <div style={{ padding: '40px 32px', display: 'flex', alignItems: 'center', gap: '12px', color: 'var(--red)' }}>
        <AlertCircle size={20} />
        <span>{error}</span>
      </div>
    );
  }

  const fmt = (n) => `₹${Number(n || 0).toLocaleString('en-IN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`;

  return (
    <div style={{ padding: '40px 32px', maxWidth: '1100px', margin: '0 auto' }} className="fade-in">
      {/* Greeting */}
      <div style={{ marginBottom: '32px' }}>
        <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '28px', fontWeight: 700 }}>
          Welcome back, <span className="gold-text">{customer?.name || 'Customer'}</span>
        </h1>
        <p style={{ color: 'var(--text-dim)', fontSize: '14px', marginTop: '4px' }}>
          {customer?.customerId} · Here's your shopping summary
        </p>
      </div>

      {/* Stats grid */}
      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(200px, 1fr))', gap: '16px' }}>
        <StatCard
          icon={IndianRupee}
          label="Total Spent"
          value={fmt(stats?.totalSpent)}
          sub="Across all purchases"
          color="var(--gold)"
        />
        <StatCard
          icon={TrendingDown}
          label="Total Saved"
          value={fmt(stats?.totalSaved)}
          sub="Via discounts & coupons"
          color="var(--green)"
        />
        <StatCard
          icon={Package}
          label="Products Bought"
          value={stats?.totalProducts ?? 0}
          sub="Total items purchased"
          color="var(--blue)"
        />
        <StatCard
          icon={Tag}
          label="Top Category"
          value={stats?.topCategory || '—'}
          sub="Most purchased category"
          color="#a78bfa"
        />
        <StatCard
          icon={Star}
          label="Loyalty Points"
          value={stats?.loyaltyPoints ?? customer?.loyaltyPoints ?? 0}
          sub="1 point per ₹100 spent"
          color="var(--gold2)"
        />
      </div>

      {/* Info note */}
      <div className="glass" style={{ marginTop: '24px', padding: '16px 20px', display: 'flex', alignItems: 'center', gap: '10px' }}>
        <Star size={16} color="var(--gold)" />
        <span style={{ fontSize: '13px', color: 'var(--text-dim)' }}>
          You earn <strong style={{ color: 'var(--gold)' }}>1 loyalty point</strong> for every ₹100 spent.
          Visit the <strong style={{ color: 'var(--text)' }}>Loyalty</strong> tab to see your balance and redemption options.
        </span>
      </div>
    </div>
  );
}
