import { useEffect, useState } from 'react';
import { Star, Info, TrendingUp, Gift, AlertCircle } from 'lucide-react';
import { getLoyalty } from '../api';

function InfoCard({ icon: Icon, title, children, color = 'var(--gold)' }) {
  return (
    <div className="glass" style={{ padding: '20px 24px' }}>
      <div style={{ display: 'flex', alignItems: 'center', gap: '10px', marginBottom: '12px' }}>
        <div style={{
          width: 32, height: 32, borderRadius: '8px',
          background: `${color}18`,
          display: 'flex', alignItems: 'center', justifyContent: 'center',
        }}>
          <Icon size={16} color={color} />
        </div>
        <span style={{ fontSize: '14px', fontWeight: 600, color: 'var(--text)' }}>{title}</span>
      </div>
      <div style={{ fontSize: '13px', color: 'var(--text-dim)', lineHeight: '1.6' }}>{children}</div>
    </div>
  );
}

export default function Loyalty() {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    getLoyalty()
      .then(res => setData(res.data))
      .catch(err => setError(err.response?.data?.error || 'Failed to load loyalty data'))
      .finally(() => setLoading(false));
  }, []);

  const points = data?.loyaltyPoints ?? data?.points ?? 0;

  return (
    <div style={{ padding: '40px 32px', maxWidth: '800px', margin: '0 auto' }} className="fade-in">
      <div style={{ marginBottom: '28px' }}>
        <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '26px', fontWeight: 700 }}>
          Loyalty <span className="gold-text">Points</span>
        </h1>
        <p style={{ color: 'var(--text-dim)', fontSize: '14px', marginTop: '4px' }}>
          Earn points with every purchase and redeem them for rewards
        </p>
      </div>

      {error && (
        <div style={{ display: 'flex', alignItems: 'center', gap: '10px', color: 'var(--red)', marginBottom: '20px' }}>
          <AlertCircle size={16} /> {error}
        </div>
      )}

      {/* Points balance hero */}
      <div className="glass-lg" style={{
        padding: '40px', textAlign: 'center', marginBottom: '24px',
        background: 'linear-gradient(135deg, rgba(212,175,55,0.06) 0%, rgba(26,26,36,0.85) 100%)',
      }}>
        {loading ? (
          <div className="skeleton" style={{ height: '80px', width: '200px', margin: '0 auto' }} />
        ) : (
          <>
            <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'center', gap: '12px', marginBottom: '8px' }}>
              <Star size={32} color="var(--gold)" fill="var(--gold)" />
              <span style={{ fontSize: '56px', fontWeight: 800, color: 'var(--gold)', lineHeight: 1 }}>
                {points.toLocaleString()}
              </span>
            </div>
            <p style={{ color: 'var(--text-dim)', fontSize: '15px' }}>loyalty points available</p>
            <div style={{ marginTop: '16px', display: 'inline-flex', alignItems: 'center', gap: '6px', padding: '6px 16px', borderRadius: '20px', background: 'rgba(212,175,55,0.1)', border: '1px solid var(--border)' }}>
              <span style={{ fontSize: '13px', color: 'var(--gold)', fontWeight: 500 }}>
                ≈ ₹{points} redemption value
              </span>
            </div>
          </>
        )}
      </div>

      {/* Info cards */}
      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(220px, 1fr))', gap: '16px' }}>
        <InfoCard icon={TrendingUp} title="How to Earn" color="var(--green)">
          You earn <strong style={{ color: 'var(--text)' }}>1 point</strong> for every{' '}
          <strong style={{ color: 'var(--text)' }}>₹100</strong> spent at checkout.
          Points are added automatically when your purchase is linked to your account.
        </InfoCard>

        <InfoCard icon={Gift} title="How to Redeem" color="var(--blue)">
          Ask the shopkeeper to apply your loyalty points at checkout.
          Each point is worth <strong style={{ color: 'var(--text)' }}>₹1</strong> off your bill.
          Redemption is processed through the shopkeeper's terminal.
        </InfoCard>

        <InfoCard icon={Info} title="Points Policy" color="#a78bfa">
          Points are tracked internally and do <strong style={{ color: 'var(--text)' }}>not appear on your bill</strong>.
          Points do not expire. Partial redemption is allowed — you can redeem any amount up to your balance.
        </InfoCard>
      </div>

      {/* Earning table */}
      <div className="glass" style={{ marginTop: '24px', padding: '20px 24px' }}>
        <div style={{ fontSize: '14px', fontWeight: 600, marginBottom: '16px', color: 'var(--text)' }}>
          Points Earning Reference
        </div>
        <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }}>
          <thead>
            <tr style={{ color: 'var(--text-dim)' }}>
              <th style={{ textAlign: 'left', paddingBottom: '10px', fontWeight: 600 }}>Purchase Amount</th>
              <th style={{ textAlign: 'right', paddingBottom: '10px', fontWeight: 600 }}>Points Earned</th>
            </tr>
          </thead>
          <tbody>
            {[100, 250, 500, 1000, 2500, 5000].map(amt => (
              <tr key={amt} style={{ borderTop: '1px solid var(--border)' }}>
                <td style={{ padding: '10px 0', color: 'var(--text)' }}>₹{amt.toLocaleString()}</td>
                <td style={{ padding: '10px 0', textAlign: 'right', color: 'var(--gold)', fontWeight: 600 }}>
                  {Math.floor(amt / 100)} pt{Math.floor(amt / 100) !== 1 ? 's' : ''}
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}
