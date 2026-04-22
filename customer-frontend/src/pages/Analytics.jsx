import { useState, useEffect } from 'react';
import { BarChart2, TrendingUp, DollarSign, Package, RefreshCw, Users, X, ChevronDown, ChevronUp } from 'lucide-react';
import { getAnalytics, getOrders, getRankedCustomers } from '../api';
import toast from 'react-hot-toast';

function StatCard({ icon: Icon, label, value, color, sub }) {
  return (
    <div className="glass" style={{ padding: '24px', display: 'flex', gap: '16px', alignItems: 'flex-start' }}>
      <div style={{
        width: 48, height: 48, borderRadius: '12px', flexShrink: 0,
        background: color + '15', border: `1px solid ${color}30`,
        display: 'flex', alignItems: 'center', justifyContent: 'center',
      }}>
        <Icon size={22} color={color} />
      </div>
      <div>
        <div style={{ fontSize: '12px', color: 'var(--text-dim)', marginBottom: '4px', textTransform: 'uppercase', letterSpacing: '0.5px' }}>{label}</div>
        <div style={{ fontSize: '24px', fontWeight: 700, color, marginBottom: '2px' }}>{value}</div>
        {sub && <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>{sub}</div>}
      </div>
    </div>
  );
}

function OrderRow({ order }) {
  const [open, setOpen] = useState(false);
  return (
    <div style={{ marginBottom: '8px', border: '1px solid var(--border)', borderRadius: '8px', overflow: 'hidden' }}>
      <button style={{
        width: '100%', display: 'flex', justifyContent: 'space-between', alignItems: 'center',
        padding: '10px 14px', background: 'var(--bg2)', border: 'none', cursor: 'pointer', color: 'inherit',
      }} onClick={() => setOpen(v => !v)}>
        <div style={{ display: 'flex', gap: '12px', alignItems: 'center' }}>
          <span style={{ color: 'var(--gold)', fontWeight: 600, fontSize: '13px' }}>#{String(order.orderId).padStart(4, '0')}</span>
          <span style={{ fontSize: '12px', color: 'var(--text-dim)' }}>{order.orderDate}</span>
          <span style={{ fontSize: '12px', color: 'var(--text-dim)' }}>{order.paymentMethod}</span>
        </div>
        <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
          <span style={{ fontWeight: 700, fontSize: '14px' }}>₹{order.totalAmount.toFixed(2)}</span>
          {open ? <ChevronUp size={14} /> : <ChevronDown size={14} />}
        </div>
      </button>
      {open && (
        <div style={{ padding: '10px 14px', borderTop: '1px solid var(--border)' }}>
          {order.products.map((p, i) => (
            <div key={i} style={{ display: 'flex', justifyContent: 'space-between', fontSize: '12px', padding: '3px 0', color: 'var(--text-dim)' }}>
              <span>{p.name} × {p.quantity}</span>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}

function CustomerModal({ entry, orders, onClose }) {
  const linkedOrders = orders.filter(o => entry.customer.orderIds?.includes(o.orderId));
  return (
    <div style={{
      position: 'fixed', inset: 0, zIndex: 1000,
      background: 'rgba(0,0,0,0.7)', display: 'flex', alignItems: 'center', justifyContent: 'center',
      padding: '24px',
    }} onClick={onClose}>
      <div className="glass-lg" style={{ maxWidth: 600, width: '100%', maxHeight: '80vh', overflow: 'hidden', display: 'flex', flexDirection: 'column' }}
           onClick={e => e.stopPropagation()}>
        <div style={{ padding: '20px 24px', borderBottom: '1px solid var(--border)', display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
          <div>
            <div style={{ fontWeight: 700, fontSize: '16px' }}>{entry.customer.name}</div>
            <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>{entry.customer.customerId} · {entry.customer.phone}</div>
          </div>
          <button className="btn btn-outline" style={{ padding: '6px 10px' }} onClick={onClose}><X size={14} /></button>
        </div>
        <div style={{ padding: '16px 24px', borderBottom: '1px solid var(--border)', display: 'flex', gap: '24px' }}>
          <div>
            <div style={{ fontSize: '11px', color: 'var(--text-dim)', textTransform: 'uppercase' }}>Total Spent</div>
            <div className="gold-text" style={{ fontWeight: 700, fontSize: '18px' }}>₹{entry.totalSpent.toFixed(2)}</div>
          </div>
          <div>
            <div style={{ fontSize: '11px', color: 'var(--text-dim)', textTransform: 'uppercase' }}>Visits</div>
            <div style={{ fontWeight: 700, fontSize: '18px' }}>{entry.visitCount}</div>
          </div>
          <div>
            <div style={{ fontSize: '11px', color: 'var(--text-dim)', textTransform: 'uppercase' }}>Loyalty Points</div>
            <div style={{ fontWeight: 700, fontSize: '18px', color: '#d4af37' }}>{entry.customer.loyaltyPoints ?? 0}</div>
          </div>
        </div>
        <div style={{ overflowY: 'auto', flex: 1, padding: '16px 24px' }}>
          <div style={{ fontWeight: 600, fontSize: '13px', marginBottom: '12px' }}>Purchase History</div>
          {linkedOrders.length === 0
            ? <p style={{ color: 'var(--text-dim)', fontSize: '13px' }}>No orders linked yet.</p>
            : [...linkedOrders].reverse().map(o => <OrderRow key={o.orderId} order={o} />)
          }
        </div>
      </div>
    </div>
  );
}

export default function Analytics() {
  const [data, setData] = useState(null);
  const [orders, setOrders] = useState([]);
  const [ranked, setRanked] = useState([]);
  const [loading, setLoading] = useState(true);
  const [selectedEntry, setSelectedEntry] = useState(null);

  const load = () => {
    setLoading(true);
    Promise.all([getAnalytics(), getOrders(), getRankedCustomers()])
      .then(([a, o, r]) => { setData(a.data); setOrders(o.data); setRanked(r.data || []); })
      .catch(() => toast.error('Failed to load analytics'))
      .finally(() => setLoading(false));
  };

  useEffect(() => { load(); }, []);

  // Build a map: orderId -> customer name (for orders linked to a registered customer)
  const orderCustomerMap = ranked.reduce((acc, entry) => {
    (entry.customer.orderIds || []).forEach(oid => { acc[oid] = entry.customer.name; });
    return acc;
  }, {});

  const methodCounts = orders.reduce((acc, o) => {
    acc[o.paymentMethod] = (acc[o.paymentMethod] || 0) + 1;
    return acc;
  }, {});

  const productFreq = orders.reduce((acc, o) => {
    o.products.forEach(p => { acc[p.name] = (acc[p.name] || 0) + p.quantity; });
    return acc;
  }, {});
  const topProducts = Object.entries(productFreq).sort((a, b) => b[1] - a[1]).slice(0, 5);
  const maxFreq = topProducts[0]?.[1] || 1;
  const METHOD_COLORS = { UPI: '#60a5fa', Card: '#a78bfa', Cash: '#4ade80' };

  return (
    <div style={{ maxWidth: 1000, margin: '0 auto', padding: '32px 24px' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '28px' }}>
        <div>
          <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '28px', fontWeight: 700 }} className="gold-text">Sales Analytics</h1>
          <p style={{ color: 'var(--text-dim)', fontSize: '14px', marginTop: '4px' }}>Real-time insights</p>
        </div>
        <button className="btn btn-outline" onClick={load} style={{ padding: '8px 12px' }}><RefreshCw size={14} /></button>
      </div>

      {loading ? (
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(220px, 1fr))', gap: '16px' }}>
          {[...Array(4)].map((_, i) => <div key={i} className="skeleton" style={{ height: '100px' }} />)}
        </div>
      ) : data && (
        <>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(220px, 1fr))', gap: '16px', marginBottom: '28px' }}>
            <StatCard icon={DollarSign} label="Total Revenue"       value={`₹${data.totalRevenue.toFixed(2)}`} color="#d4af37" />
            <StatCard icon={TrendingUp} label="Total GST Collected" value={`₹${data.totalGST.toFixed(2)}`}    color="#60a5fa" sub="CGST + SGST (9% each)" />
            <StatCard icon={Package}   label="Total Orders"         value={data.orderCount}                    color="#4ade80" />
            <StatCard icon={BarChart2} label="Top Product"          value={data.mostPurchased || 'N/A'}        color="#a78bfa" />
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '20px' }}>
            <div className="glass" style={{ padding: '20px' }}>
              <div style={{ fontWeight: 600, marginBottom: '16px', fontSize: '14px' }}>Top Products by Quantity</div>
              {topProducts.length === 0
                ? <p style={{ color: 'var(--text-dim)', fontSize: '13px' }}>No data yet</p>
                : topProducts.map(([name, qty]) => (
                  <div key={name} style={{ marginBottom: '12px' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between', fontSize: '13px', marginBottom: '5px' }}>
                      <span>{name}</span>
                      <span className="gold-text" style={{ fontWeight: 600 }}>{qty}</span>
                    </div>
                    <div style={{ height: '6px', background: 'var(--bg2)', borderRadius: '3px', overflow: 'hidden' }}>
                      <div style={{ height: '100%', borderRadius: '3px', width: `${(qty / maxFreq) * 100}%`, background: 'linear-gradient(90deg,#d4af37,#f0d060)', transition: 'width 0.6s ease' }} />
                    </div>
                  </div>
                ))}
            </div>

            <div className="glass" style={{ padding: '20px' }}>
              <div style={{ fontWeight: 600, marginBottom: '16px', fontSize: '14px' }}>Payment Methods</div>
              {Object.keys(methodCounts).length === 0
                ? <p style={{ color: 'var(--text-dim)', fontSize: '13px' }}>No data yet</p>
                : Object.entries(methodCounts).map(([m, count]) => {
                  const total = Object.values(methodCounts).reduce((a, b) => a + b, 0);
                  const pct = Math.round((count / total) * 100);
                  return (
                    <div key={m} style={{ marginBottom: '14px' }}>
                      <div style={{ display: 'flex', justifyContent: 'space-between', fontSize: '13px', marginBottom: '5px' }}>
                        <span style={{ color: METHOD_COLORS[m] || '#888' }}>{m}</span>
                        <span style={{ color: 'var(--text-dim)' }}>{count} orders ({pct}%)</span>
                      </div>
                      <div style={{ height: '6px', background: 'var(--bg2)', borderRadius: '3px', overflow: 'hidden' }}>
                        <div style={{ height: '100%', borderRadius: '3px', width: `${pct}%`, background: METHOD_COLORS[m] || '#888', transition: 'width 0.6s ease' }} />
                      </div>
                    </div>
                  );
                })}
            </div>

            <div className="glass" style={{ padding: '20px', gridColumn: '1 / -1' }}>
              <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '16px' }}>
                <Users size={16} color="var(--gold)" />
                <div style={{ fontWeight: 600, fontSize: '14px' }}>Customer Rankings</div>
                <span style={{ fontSize: '12px', color: 'var(--text-dim)', marginLeft: '4px' }}>Click a row to view order history</span>
              </div>
              {ranked.length === 0
                ? <p style={{ color: 'var(--text-dim)', fontSize: '13px' }}>No customers yet</p>
                : (
                  <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }}>
                    <thead>
                      <tr style={{ borderBottom: '1px solid var(--border)' }}>
                        {['Rank', 'Name', 'Phone', 'Total Spent', 'Visits'].map(h => (
                          <th key={h} style={{ padding: '8px 12px', textAlign: 'left', color: 'var(--text-dim)', fontWeight: 500 }}>{h}</th>
                        ))}
                      </tr>
                    </thead>
                    <tbody>
                      {ranked.map(entry => (
                        <tr key={entry.customer.customerId}
                            onClick={() => setSelectedEntry(entry)}
                            style={{ borderBottom: '1px solid rgba(212,175,55,0.05)', cursor: 'pointer' }}
                            onMouseEnter={e => e.currentTarget.style.background = 'rgba(212,175,55,0.04)'}
                            onMouseLeave={e => e.currentTarget.style.background = 'transparent'}>
                          <td style={{ padding: '10px 12px' }}>
                            <span style={{
                              display: 'inline-flex', alignItems: 'center', justifyContent: 'center',
                              width: 24, height: 24, borderRadius: '50%', fontSize: '12px', fontWeight: 700,
                              background: entry.rank <= 3 ? 'rgba(212,175,55,0.15)' : 'var(--bg2)',
                              color: entry.rank <= 3 ? 'var(--gold)' : 'var(--text-dim)',
                            }}>{entry.rank}</span>
                          </td>
                          <td style={{ padding: '10px 12px', fontWeight: 500 }}>{entry.customer.name}</td>
                          <td style={{ padding: '10px 12px', color: 'var(--text-dim)' }}>{entry.customer.phone}</td>
                          <td style={{ padding: '10px 12px' }}><span className="gold-text" style={{ fontWeight: 600 }}>₹{entry.totalSpent.toFixed(2)}</span></td>
                          <td style={{ padding: '10px 12px', color: 'var(--text-dim)' }}>{entry.visitCount}</td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                )}
            </div>

            <div className="glass" style={{ padding: '20px', gridColumn: '1 / -1' }}>
              <div style={{ fontWeight: 600, marginBottom: '16px', fontSize: '14px' }}>Recent Orders</div>
              {orders.length === 0
                ? <p style={{ color: 'var(--text-dim)', fontSize: '13px' }}>No orders yet</p>
                : (
                  <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }}>
                    <thead>
                      <tr style={{ borderBottom: '1px solid var(--border)' }}>
                        {['Order', 'Date', 'Customer', 'Products', 'Method', 'Total'].map(h => (
                          <th key={h} style={{ padding: '8px 12px', textAlign: 'left', color: 'var(--text-dim)', fontWeight: 500 }}>{h}</th>
                        ))}
                      </tr>
                    </thead>
                    <tbody>
                      {[...orders].reverse().slice(0, 8).map(o => (
                        <tr key={o.orderId} style={{ borderBottom: '1px solid rgba(212,175,55,0.05)' }}>
                          <td style={{ padding: '10px 12px', color: 'var(--gold)' }}>#{String(o.orderId).padStart(4, '0')}</td>
                          <td style={{ padding: '10px 12px', color: 'var(--text-dim)' }}>{o.orderDate}</td>
                          <td style={{ padding: '10px 12px' }}>
                            {orderCustomerMap[o.orderId]
                              ? <span style={{ fontWeight: 500 }}>{orderCustomerMap[o.orderId]}</span>
                              : <span style={{ color: 'var(--text-dim)', fontStyle: 'italic' }}>Guest</span>}
                          </td>
                          <td style={{ padding: '10px 12px' }}>{o.products.map(p => p.name).join(', ')}</td>
                          <td style={{ padding: '10px 12px' }}><span style={{ color: METHOD_COLORS[o.paymentMethod] || '#888' }}>{o.paymentMethod}</span></td>
                          <td style={{ padding: '10px 12px', fontWeight: 600 }}>₹{o.totalAmount.toFixed(2)}</td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                )}
            </div>
          </div>
        </>
      )}

      {selectedEntry && (
        <CustomerModal entry={selectedEntry} orders={orders} onClose={() => setSelectedEntry(null)} />
      )}
    </div>
  );
}
