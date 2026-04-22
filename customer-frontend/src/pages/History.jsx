import { useEffect, useState } from 'react';
import { ChevronDown, ChevronUp, ShoppingBag, AlertCircle, Calendar, IndianRupee } from 'lucide-react';
import { getOrders } from '../api';

function OrderRow({ order }) {
  const [open, setOpen] = useState(false);

  const fmt = (n) => `₹${Number(n || 0).toLocaleString('en-IN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`;

  return (
    <div className="glass" style={{ overflow: 'hidden' }}>
      {/* Header row */}
      <button
        onClick={() => setOpen(o => !o)}
        style={{
          width: '100%', background: 'none', border: 'none', cursor: 'pointer',
          padding: '16px 20px', display: 'flex', alignItems: 'center', gap: '16px',
          color: 'var(--text)', fontFamily: 'Inter, sans-serif',
        }}
      >
        <div style={{
          width: 36, height: 36, borderRadius: '10px',
          background: 'rgba(212,175,55,0.08)',
          display: 'flex', alignItems: 'center', justifyContent: 'center', flexShrink: 0,
        }}>
          <ShoppingBag size={16} color="var(--gold)" />
        </div>

        <div style={{ flex: 1, textAlign: 'left' }}>
          <div style={{ fontSize: '14px', fontWeight: 600 }}>Order #{order.orderId}</div>
          <div style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'flex', alignItems: 'center', gap: '6px', marginTop: '2px' }}>
            <Calendar size={11} />
            {order.date || 'N/A'}
            {order.paymentMethod && (
              <span className="badge badge-gold" style={{ marginLeft: '4px' }}>{order.paymentMethod}</span>
            )}
          </div>
        </div>

        <div style={{ textAlign: 'right', marginRight: '12px' }}>
          <div style={{ fontSize: '15px', fontWeight: 700, color: 'var(--gold)' }}>{fmt(order.totalAmount)}</div>
          {order.discount > 0 && (
            <div style={{ fontSize: '11px', color: 'var(--green)' }}>saved {fmt(order.discount)}</div>
          )}
        </div>

        {open ? <ChevronUp size={16} color="var(--text-dim)" /> : <ChevronDown size={16} color="var(--text-dim)" />}
      </button>

      {/* Expanded items */}
      {open && (
        <div style={{ borderTop: '1px solid var(--border)', padding: '16px 20px' }} className="fade-in">
          {order.items && order.items.length > 0 ? (
            <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }}>
              <thead>
                <tr style={{ color: 'var(--text-dim)' }}>
                  <th style={{ textAlign: 'left', paddingBottom: '8px', fontWeight: 600 }}>Item</th>
                  <th style={{ textAlign: 'center', paddingBottom: '8px', fontWeight: 600 }}>Qty</th>
                  <th style={{ textAlign: 'right', paddingBottom: '8px', fontWeight: 600 }}>Price</th>
                  <th style={{ textAlign: 'right', paddingBottom: '8px', fontWeight: 600 }}>Subtotal</th>
                </tr>
              </thead>
              <tbody>
                {order.items.map((item, i) => (
                  <tr key={i} style={{ borderTop: '1px solid var(--border)' }}>
                    <td style={{ padding: '8px 0', color: 'var(--text)' }}>{item.name}</td>
                    <td style={{ padding: '8px 0', textAlign: 'center', color: 'var(--text-dim)' }}>{item.quantity}</td>
                    <td style={{ padding: '8px 0', textAlign: 'right', color: 'var(--text-dim)' }}>{fmt(item.price)}</td>
                    <td style={{ padding: '8px 0', textAlign: 'right', color: 'var(--text)', fontWeight: 500 }}>
                      {fmt((item.price || 0) * (item.quantity || 1))}
                    </td>
                  </tr>
                ))}
              </tbody>
              <tfoot>
                <tr style={{ borderTop: '1px solid var(--border)' }}>
                  <td colSpan={3} style={{ paddingTop: '10px', fontWeight: 600, color: 'var(--text-dim)' }}>Total</td>
                  <td style={{ paddingTop: '10px', textAlign: 'right', fontWeight: 700, color: 'var(--gold)', fontSize: '14px' }}>
                    {fmt(order.totalAmount)}
                  </td>
                </tr>
              </tfoot>
            </table>
          ) : (
            <p style={{ color: 'var(--text-dim)', fontSize: '13px' }}>No item details available for this order.</p>
          )}
        </div>
      )}
    </div>
  );
}

export default function History() {
  const [orders, setOrders] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    getOrders()
      .then(res => setOrders(res.data.orders || res.data || []))
      .catch(err => setError(err.response?.data?.error || 'Failed to load orders'))
      .finally(() => setLoading(false));
  }, []);

  if (loading) {
    return (
      <div style={{ padding: '40px 32px', display: 'flex', flexDirection: 'column', gap: '12px' }}>
        {[...Array(4)].map((_, i) => <div key={i} className="skeleton" style={{ height: '72px' }} />)}
      </div>
    );
  }

  return (
    <div style={{ padding: '40px 32px', maxWidth: '900px', margin: '0 auto' }} className="fade-in">
      <div style={{ marginBottom: '28px' }}>
        <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '26px', fontWeight: 700 }}>
          Purchase <span className="gold-text">History</span>
        </h1>
        <p style={{ color: 'var(--text-dim)', fontSize: '14px', marginTop: '4px' }}>
          {orders.length} order{orders.length !== 1 ? 's' : ''} — click any row to expand
        </p>
      </div>

      {error && (
        <div style={{ display: 'flex', alignItems: 'center', gap: '10px', color: 'var(--red)', marginBottom: '20px' }}>
          <AlertCircle size={16} /> {error}
        </div>
      )}

      {orders.length === 0 && !error ? (
        <div className="glass" style={{ padding: '48px', textAlign: 'center' }}>
          <ShoppingBag size={40} color="var(--text-dim)" style={{ margin: '0 auto 16px' }} />
          <p style={{ color: 'var(--text-dim)', fontSize: '15px' }}>No orders yet.</p>
          <p style={{ color: 'var(--text-dim)', fontSize: '13px', marginTop: '6px' }}>
            Your purchase history will appear here after your first linked transaction.
          </p>
        </div>
      ) : (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '10px' }}>
          {orders.map((order, i) => (
            <OrderRow key={order.orderId ?? i} order={order} />
          ))}
        </div>
      )}

      {/* Summary bar */}
      {orders.length > 0 && (
        <div className="glass" style={{ marginTop: '20px', padding: '14px 20px', display: 'flex', alignItems: 'center', gap: '8px' }}>
          <IndianRupee size={15} color="var(--gold)" />
          <span style={{ fontSize: '13px', color: 'var(--text-dim)' }}>
            Total across all orders:{' '}
            <strong style={{ color: 'var(--gold)' }}>
              ₹{orders.reduce((s, o) => s + (o.totalAmount || 0), 0)
                .toLocaleString('en-IN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
            </strong>
          </span>
        </div>
      )}
    </div>
  );
}
