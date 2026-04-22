import { useState, useEffect } from 'react';
import { History, Download, ChevronDown, ChevronUp } from 'lucide-react';
import { getOrders } from '../api';
import toast from 'react-hot-toast';

const METHOD_COLORS = { UPI: '#60a5fa', Card: '#a78bfa', Cash: '#4ade80' };

function OrderCard({ order }) {
  const [open, setOpen] = useState(false);

  const downloadReceipt = () => {
    const content = [
      '========================================',
      `  ORDER #${String(order.orderId).padStart(4,'0')}`,
      '========================================',
      `Date: ${order.orderDate}   Time: ${order.orderTime}`,
      '',
      'Products:',
      ...order.products.map(p => `  ${p.name}  x${p.quantity}`),
      '',
      `Total: ₹${order.totalAmount.toFixed(2)}`,
      `Payment: ${order.paymentMethod}`,
      '========================================',
    ].join('\n');
    const blob = new Blob([content], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url; a.download = `order_${order.orderId}.txt`; a.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div className="glass fade-in" style={{ overflow: 'hidden' }}>
      <div style={{ padding: '16px 20px', display: 'flex', alignItems: 'center', gap: '16px', cursor: 'pointer' }}
           onClick={() => setOpen(o => !o)}>
        <div style={{
          width: 44, height: 44, borderRadius: '10px',
          background: 'var(--gold-dim)', border: '1px solid var(--border)',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
          fontFamily: 'Playfair Display, serif', fontWeight: 700, color: 'var(--gold)', fontSize: '13px',
          flexShrink: 0,
        }}>
          #{String(order.orderId).padStart(4,'0')}
        </div>
        <div style={{ flex: 1 }}>
          <div style={{ fontWeight: 600, marginBottom: '3px' }}>
            {order.products.map(p => p.name).join(', ')}
          </div>
          <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>
            {order.orderDate} · {order.orderTime}
          </div>
        </div>
        <span className="badge" style={{
          background: (METHOD_COLORS[order.paymentMethod] || '#888') + '15',
          color: METHOD_COLORS[order.paymentMethod] || '#888',
          border: `1px solid ${(METHOD_COLORS[order.paymentMethod] || '#888')}30`,
        }}>{order.paymentMethod}</span>
        <span className="gold-text" style={{ fontWeight: 700, fontSize: '16px', minWidth: '90px', textAlign: 'right' }}>
          ₹{order.totalAmount.toFixed(2)}
        </span>
        {open ? <ChevronUp size={16} color="var(--text-dim)" /> : <ChevronDown size={16} color="var(--text-dim)" />}
      </div>

      {open && (
        <div style={{ borderTop: '1px solid var(--border)', padding: '16px 20px', background: 'rgba(0,0,0,0.2)' }}>
          <div style={{ marginBottom: '12px' }}>
            <div style={{ fontSize: '12px', color: 'var(--text-dim)', marginBottom: '8px', fontWeight: 600, textTransform: 'uppercase', letterSpacing: '0.5px' }}>Items</div>
            {order.products.map((p, i) => (
              <div key={i} style={{ display: 'flex', justifyContent: 'space-between', padding: '4px 0', fontSize: '13px' }}>
                <span>{p.name}</span>
                <span style={{ color: 'var(--text-dim)' }}>×{p.quantity}</span>
              </div>
            ))}
          </div>
          <button className="btn btn-outline" onClick={downloadReceipt} style={{ fontSize: '12px', padding: '7px 14px' }}>
            <Download size={13} /> Download Receipt
          </button>
        </div>
      )}
    </div>
  );
}

export default function Orders() {
  const [orders, setOrders] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    getOrders()
      .then(r => setOrders([...r.data].reverse()))
      .catch(() => toast.error('Failed to load orders'))
      .finally(() => setLoading(false));
  }, []);

  return (
    <div style={{ maxWidth: 800, margin: '0 auto', padding: '32px 24px' }}>
      <div style={{ display: 'flex', alignItems: 'center', gap: '12px', marginBottom: '28px' }}>
        <History size={28} color="var(--gold)" />
        <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '28px', fontWeight: 700 }}
            className="gold-text">Order History</h1>
        <span className="badge badge-gold" style={{ marginLeft: 'auto' }}>{orders.length} orders</span>
      </div>

      {loading ? (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
          {[...Array(4)].map((_, i) => <div key={i} className="skeleton" style={{ height: '76px' }} />)}
        </div>
      ) : orders.length === 0 ? (
        <div style={{ textAlign: 'center', padding: '80px 0', color: 'var(--text-dim)' }}>
          <History size={48} style={{ marginBottom: '16px', opacity: 0.3 }} />
          <p>No orders yet</p>
        </div>
      ) : (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '10px' }}>
          {orders.map(o => <OrderCard key={o.orderId} order={o} />)}
        </div>
      )}
    </div>
  );
}
