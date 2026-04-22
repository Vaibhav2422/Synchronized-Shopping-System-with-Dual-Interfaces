import { useState, useEffect, useRef } from 'react';
import { useNavigate } from 'react-router-dom';
import { CreditCard, Smartphone, Banknote, CheckCircle, Download, Star, Search, UserPlus, UserCheck, X } from 'lucide-react';
import { getBill, getLoyalty, checkout, searchCustomers, registerCustomer } from '../api';
import toast from 'react-hot-toast';

const METHODS = [
  { id: 'UPI',  label: 'UPI',              icon: Smartphone, color: '#60a5fa' },
  { id: 'Card', label: 'Credit/Debit Card', icon: CreditCard, color: '#a78bfa' },
  { id: 'Cash', label: 'Cash',             icon: Banknote,   color: '#4ade80' },
];

// ── Customer Selection Step ───────────────────────────────────────────────────
function CustomerStep({ onSelect, onSkip }) {
  const [mode, setMode] = useState(null); // null | 'search' | 'new'
  const [query, setQuery] = useState('');
  const [results, setResults] = useState([]);
  const [searching, setSearching] = useState(false);
  const [newForm, setNewForm] = useState({ name: '', phone: '', password: '' });
  const [registering, setRegistering] = useState(false);
  const debounceRef = useRef(null);

  const handleQueryChange = (val) => {
    setQuery(val);
    clearTimeout(debounceRef.current);
    if (!val.trim()) { setResults([]); return; }
    debounceRef.current = setTimeout(async () => {
      setSearching(true);
      try {
        const res = await searchCustomers(val);
        setResults(res.data || []);
      } catch { setResults([]); }
      finally { setSearching(false); }
    }, 300);
  };

  const handleRegister = async () => {
    if (!newForm.name.trim() || !newForm.phone.trim() || !newForm.password.trim()) {
      toast.error('Name, phone, and password are required');
      return;
    }
    setRegistering(true);
    try {
      const res = await registerCustomer(newForm);
      toast.success(`Customer registered! ID: ${res.data.customerId}`);
      onSelect(res.data.customer);
    } catch (e) {
      toast.error(e.response?.data?.error || 'Registration failed');
    } finally { setRegistering(false); }
  };

  return (
    <div className="glass" style={{ padding: '24px' }}>
      <div style={{ fontWeight: 600, marginBottom: '18px', fontSize: '15px' }}>Customer</div>

      {/* Three options */}
      {!mode && (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '10px' }}>
          <button className="btn btn-outline" style={{ justifyContent: 'flex-start', gap: '10px' }}
                  onClick={onSkip}>
            <X size={16} /> Skip — Guest Checkout
          </button>
          <button className="btn btn-outline" style={{ justifyContent: 'flex-start', gap: '10px' }}
                  onClick={() => setMode('search')}>
            <Search size={16} /> Select Existing Customer
          </button>
          <button className="btn btn-outline" style={{ justifyContent: 'flex-start', gap: '10px' }}
                  onClick={() => setMode('new')}>
            <UserPlus size={16} /> Add New Customer
          </button>
        </div>
      )}

      {/* Search existing */}
      {mode === 'search' && (
        <div>
          <div style={{ display: 'flex', gap: '8px', marginBottom: '12px' }}>
            <input className="input" placeholder="Search by name, phone, or ID"
                   value={query} onChange={e => handleQueryChange(e.target.value)}
                   style={{ flex: 1 }} autoFocus />
            <button className="btn btn-outline" style={{ padding: '8px 12px' }}
                    onClick={() => { setMode(null); setQuery(''); setResults([]); }}>
              <X size={14} />
            </button>
          </div>
          {searching && <div style={{ color: 'var(--text-dim)', fontSize: '13px' }}>Searching…</div>}
          {!searching && query && results.length === 0 && (
            <div style={{ color: 'var(--text-dim)', fontSize: '13px', marginBottom: '10px' }}>
              No customer found.{' '}
              <button style={{ background: 'none', border: 'none', color: 'var(--gold)', cursor: 'pointer', fontSize: '13px', padding: 0 }}
                      onClick={() => { setMode('new'); setQuery(''); }}>
                Add New Customer
              </button>
            </div>
          )}
          <div style={{ display: 'flex', flexDirection: 'column', gap: '6px' }}>
            {results.map(c => (
              <button key={c.customerId}
                      onClick={() => onSelect(c)}
                      style={{
                        display: 'flex', alignItems: 'center', gap: '12px',
                        padding: '10px 14px', borderRadius: '8px', cursor: 'pointer',
                        border: '1px solid var(--border)', background: 'var(--bg2)',
                        textAlign: 'left', width: '100%', transition: 'border-color 0.2s',
                      }}
                      onMouseEnter={e => e.currentTarget.style.borderColor = 'var(--gold)'}
                      onMouseLeave={e => e.currentTarget.style.borderColor = 'var(--border)'}>
                <UserCheck size={16} color="var(--gold)" />
                <div>
                  <div style={{ fontWeight: 500, fontSize: '14px', color: 'var(--text)' }}>{c.name}</div>
                  <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>{c.customerId} · {c.phone}</div>
                </div>
              </button>
            ))}
          </div>
        </div>
      )}

      {/* Add new */}
      {mode === 'new' && (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
          <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '4px' }}>
            <span style={{ fontSize: '13px', color: 'var(--text-dim)' }}>New Customer</span>
            <button className="btn btn-outline" style={{ padding: '4px 8px', fontSize: '12px' }}
                    onClick={() => setMode(null)}>
              <X size={12} /> Back
            </button>
          </div>
          <div>
            <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '5px' }}>Name *</label>
            <input className="input" placeholder="Full name"
                   value={newForm.name} onChange={e => setNewForm(f => ({ ...f, name: e.target.value }))} autoFocus />
          </div>
          <div>
            <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '5px' }}>Phone *</label>
            <input className="input" placeholder="Phone number"
                   value={newForm.phone} onChange={e => setNewForm(f => ({ ...f, phone: e.target.value }))} />
          </div>
          <div>
            <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '5px' }}>Password *</label>
            <input className="input" type="password" placeholder="Set a password"
                   value={newForm.password} onChange={e => setNewForm(f => ({ ...f, password: e.target.value }))} />
          </div>
          <button className="btn btn-gold" onClick={handleRegister} disabled={registering}
                  style={{ justifyContent: 'center' }}>
            {registering ? <><span className="spinner" /> Registering…</> : 'Register & Select'}
          </button>
        </div>
      )}
    </div>
  );
}

// ── Main Checkout ─────────────────────────────────────────────────────────────
export default function Checkout() {
  const nav = useNavigate();
  const [bill, setBill] = useState(null);
  const [loyalty, setLoyalty] = useState(null);
  const [method, setMethod] = useState('UPI');
  const [redeemLoyalty, setRedeemLoyalty] = useState(false);
  const [form, setForm] = useState({ upiId: '', cardNumber: '', cardHolder: '', cashTendered: '' });
  const [processing, setProcessing] = useState(false);
  const [receipt, setReceipt] = useState(null);
  const [selectedCustomer, setSelectedCustomer] = useState(null); // null = not chosen yet
  const [customerSkipped, setCustomerSkipped] = useState(false);

  const customerChosen = customerSkipped || selectedCustomer !== null;

  useEffect(() => {
    Promise.all([getBill(), getLoyalty()])
      .then(([b, l]) => { setBill(b.data); setLoyalty(l.data); })
      .catch(() => toast.error('Failed to load bill'));
  }, []);

  useEffect(() => {
    getBill(redeemLoyalty).then(r => setBill(r.data)).catch(() => {});
  }, [redeemLoyalty]);

  const handleCheckout = async () => {
    setProcessing(true);
    try {
      const payload = {
        method, redeemLoyalty, ...form,
        cashTendered: parseFloat(form.cashTendered) || 0,
        ...(selectedCustomer ? { customerId: selectedCustomer.customerId } : {}),
      };
      const res = await checkout(payload);
      setReceipt(res.data);
      toast.success(`Order #${res.data.orderId} placed!`);
    } catch (e) {
      toast.error(e.response?.data?.error || 'Checkout failed');
    } finally { setProcessing(false); }
  };

  const downloadReceipt = () => {
    if (!receipt) return;
    const blob = new Blob([receipt.receipt], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url; a.download = `receipt_${receipt.orderId}.txt`; a.click();
    URL.revokeObjectURL(url);
  };

  if (receipt) return (
    <div style={{ maxWidth: 560, margin: '60px auto', padding: '0 24px' }}>
      <div className="glass-lg" style={{ padding: '40px', textAlign: 'center' }}>
        <div style={{
          width: 72, height: 72, borderRadius: '50%',
          background: 'rgba(74,222,128,0.1)', border: '2px solid var(--green)',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
          margin: '0 auto 20px',
        }}>
          <CheckCircle size={36} color="var(--green)" />
        </div>
        <h2 style={{ fontFamily: 'Playfair Display, serif', fontSize: '26px', marginBottom: '8px' }}
            className="gold-text">Order Confirmed!</h2>
        <p style={{ color: 'var(--text-dim)', marginBottom: '24px' }}>
          Order #{receipt.orderId} placed successfully
        </p>
        <div className="glass" style={{ padding: '16px', marginBottom: '20px', textAlign: 'left' }}>
          <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '8px' }}>
            <span style={{ color: 'var(--text-dim)', fontSize: '13px' }}>Total Paid</span>
            <span className="gold-text" style={{ fontWeight: 700, fontSize: '18px' }}>
              ₹{receipt.finalTotal?.toFixed(2)}
            </span>
          </div>
          {receipt.earnedPoints > 0 && (
            <div style={{ display: 'flex', alignItems: 'center', gap: '6px', color: 'var(--gold)', fontSize: '13px' }}>
              <Star size={14} />
              +{receipt.earnedPoints} loyalty points earned (Total: {receipt.totalPoints})
            </div>
          )}
        </div>
        <div style={{ display: 'flex', gap: '12px', justifyContent: 'center' }}>
          <button className="btn btn-outline" onClick={downloadReceipt}>
            <Download size={15} /> Download Receipt
          </button>
          <button className="btn btn-gold" onClick={() => nav('/')}>
            Continue Shopping
          </button>
        </div>
      </div>
    </div>
  );

  return (
    <div style={{ maxWidth: 900, margin: '0 auto', padding: '32px 24px' }}>
      <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '28px', fontWeight: 700, marginBottom: '28px' }}
          className="gold-text">Checkout</h1>

      <div style={{ display: 'grid', gridTemplateColumns: '1fr 340px', gap: '24px' }}>
        {/* Left: customer + payment */}
        <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>

          {/* Step 1: Customer Selection */}
          {!customerChosen ? (
            <CustomerStep
              onSelect={c => { setSelectedCustomer(c); }}
              onSkip={() => setCustomerSkipped(true)}
            />
          ) : (
            <div className="glass" style={{ padding: '16px', display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
              <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
                <UserCheck size={18} color={selectedCustomer ? 'var(--gold)' : 'var(--text-dim)'} />
                <div>
                  {selectedCustomer ? (
                    <>
                      <div style={{ fontWeight: 600, fontSize: '14px' }}>{selectedCustomer.name}</div>
                      <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>{selectedCustomer.customerId}</div>
                    </>
                  ) : (
                    <div style={{ color: 'var(--text-dim)', fontSize: '14px' }}>Guest Checkout</div>
                  )}
                </div>
              </div>
              <button className="btn btn-outline" style={{ padding: '4px 10px', fontSize: '12px' }}
                      onClick={() => { setSelectedCustomer(null); setCustomerSkipped(false); }}>
                Change
              </button>
            </div>
          )}

          {/* Step 2: Payment Method (only shown after customer step) */}
          {customerChosen && (
            <>
              <div className="glass" style={{ padding: '20px' }}>
                <div style={{ fontWeight: 600, marginBottom: '16px' }}>Payment Method</div>
                <div style={{ display: 'flex', flexDirection: 'column', gap: '10px' }}>
                  {METHODS.map(({ id, label, icon: Icon, color }) => (
                    <label key={id} style={{
                      display: 'flex', alignItems: 'center', gap: '14px',
                      padding: '14px 16px', borderRadius: '10px', cursor: 'pointer',
                      border: `1px solid ${method === id ? color + '40' : 'var(--border)'}`,
                      background: method === id ? color + '08' : 'transparent',
                      transition: 'all 0.2s',
                    }}>
                      <input type="radio" name="method" value={id} checked={method === id}
                             onChange={() => setMethod(id)} style={{ accentColor: color }} />
                      <Icon size={20} color={color} />
                      <span style={{ fontWeight: 500 }}>{label}</span>
                    </label>
                  ))}
                </div>
              </div>

              <div className="glass" style={{ padding: '20px' }}>
                <div style={{ fontWeight: 600, marginBottom: '16px' }}>Payment Details</div>
                {method === 'UPI' && (
                  <div>
                    <label style={{ fontSize: '13px', color: 'var(--text-dim)', display: 'block', marginBottom: '6px' }}>UPI ID</label>
                    <input className="input" placeholder="yourname@upi"
                           value={form.upiId} onChange={e => setForm(f => ({ ...f, upiId: e.target.value }))} />
                  </div>
                )}
                {method === 'Card' && (
                  <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
                    <div>
                      <label style={{ fontSize: '13px', color: 'var(--text-dim)', display: 'block', marginBottom: '6px' }}>Card Number</label>
                      <input className="input" placeholder="1234 5678 9012 3456" maxLength={16}
                             value={form.cardNumber} onChange={e => setForm(f => ({ ...f, cardNumber: e.target.value.replace(/\D/g,'') }))} />
                    </div>
                    <div>
                      <label style={{ fontSize: '13px', color: 'var(--text-dim)', display: 'block', marginBottom: '6px' }}>Cardholder Name</label>
                      <input className="input" placeholder="Full Name"
                             value={form.cardHolder} onChange={e => setForm(f => ({ ...f, cardHolder: e.target.value }))} />
                    </div>
                  </div>
                )}
                {method === 'Cash' && (
                  <div>
                    <label style={{ fontSize: '13px', color: 'var(--text-dim)', display: 'block', marginBottom: '6px' }}>
                      Cash Tendered (Total: ₹{bill?.finalTotal?.toFixed(2)})
                    </label>
                    <input className="input" type="number" placeholder="Enter amount"
                           value={form.cashTendered} onChange={e => setForm(f => ({ ...f, cashTendered: e.target.value }))} />
                    {form.cashTendered && bill && parseFloat(form.cashTendered) >= bill.finalTotal && (
                      <div style={{ marginTop: '8px', color: 'var(--green)', fontSize: '13px' }}>
                        Change: ₹{(parseFloat(form.cashTendered) - bill.finalTotal).toFixed(2)}
                      </div>
                    )}
                  </div>
                )}
              </div>

              {loyalty && loyalty.points > 0 && (
                <div className="glass" style={{ padding: '16px' }}>
                  <label style={{ display: 'flex', alignItems: 'center', gap: '10px', cursor: 'pointer' }}>
                    <input type="checkbox" checked={redeemLoyalty} onChange={e => setRedeemLoyalty(e.target.checked)}
                           style={{ accentColor: 'var(--gold)', width: 16, height: 16 }} />
                    <Star size={16} color="var(--gold)" />
                    <div>
                      <div style={{ fontWeight: 500, fontSize: '14px' }}>Redeem {loyalty.points} loyalty points</div>
                      <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>Save ₹{loyalty.rupeeValue?.toFixed(2)}</div>
                    </div>
                  </label>
                </div>
              )}
            </>
          )}
        </div>

        {/* Right: order summary */}
        <div style={{ display: 'flex', flexDirection: 'column', gap: '16px' }}>
          {bill && (
            <div className="glass" style={{ padding: '20px' }}>
              <div style={{ fontWeight: 600, marginBottom: '16px' }}>Order Summary</div>

              {/* Customer name in bill preview */}
              {selectedCustomer && (
                <div style={{
                  display: 'flex', alignItems: 'center', gap: '8px',
                  padding: '8px 10px', borderRadius: '8px', marginBottom: '12px',
                  background: 'rgba(212,175,55,0.06)', border: '1px solid rgba(212,175,55,0.2)',
                }}>
                  <UserCheck size={14} color="var(--gold)" />
                  <div>
                    <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>Customer</div>
                    <div style={{ fontSize: '13px', fontWeight: 600 }}>{selectedCustomer.name}</div>
                    <div style={{ fontSize: '11px', color: 'var(--text-dim)' }}>{selectedCustomer.customerId}</div>
                  </div>
                </div>
              )}

              {[
                { label: 'Subtotal',          val: bill.subtotal,              color: 'var(--text)', pre: '₹' },
                { label: 'Product Discounts', val: -bill.productDiscounts,     color: 'var(--green)', pre: '₹' },
                { label: 'Cart Discount',     val: -bill.cartDiscount,         color: 'var(--green)', pre: '₹' },
                { label: 'Coupon Discounts',  val: -bill.stackedCouponDiscount,color: 'var(--green)', pre: '₹' },
                { label: 'Loyalty Discount',  val: -bill.loyaltyDiscount,      color: 'var(--gold)',  pre: '₹' },
              ].filter(r => r.val !== 0).map(r => (
                <div key={r.label} style={{ display: 'flex', justifyContent: 'space-between', padding: '5px 0', fontSize: '13px' }}>
                  <span style={{ color: 'var(--text-dim)' }}>{r.label}</span>
                  <span style={{ color: r.color }}>{r.pre}{Math.abs(r.val).toFixed(2)}</span>
                </div>
              ))}
              <div className="divider" />
              <div style={{ display: 'flex', justifyContent: 'space-between', fontSize: '13px', marginBottom: '4px' }}>
                <span style={{ color: 'var(--text-dim)' }}>CGST (9%)</span>
                <span>₹{bill.cgst?.toFixed(2)}</span>
              </div>
              <div style={{ display: 'flex', justifyContent: 'space-between', fontSize: '13px', marginBottom: '12px' }}>
                <span style={{ color: 'var(--text-dim)' }}>SGST (9%)</span>
                <span>₹{bill.sgst?.toFixed(2)}</span>
              </div>
              <div className="divider" />
              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <span style={{ fontWeight: 700 }}>Total</span>
                <span className="gold-text" style={{ fontWeight: 700, fontSize: '22px' }}>
                  ₹{bill.finalTotal?.toFixed(2)}
                </span>
              </div>
              {bill.appliedCoupons?.length > 0 && (
                <div style={{ marginTop: '10px', display: 'flex', flexWrap: 'wrap', gap: '4px' }}>
                  {bill.appliedCoupons.map(c => (
                    <span key={c} className="badge badge-green">{c}</span>
                  ))}
                </div>
              )}
            </div>
          )}

          {customerChosen && (
            <button className="btn btn-gold" onClick={handleCheckout} disabled={processing}
                    style={{ width: '100%', justifyContent: 'center', padding: '16px', fontSize: '15px' }}>
              {processing ? <><span className="spinner" /> Processing...</> : `Pay ₹${bill?.finalTotal?.toFixed(2) || '...'}`}
            </button>
          )}
        </div>
      </div>
    </div>
  );
}
