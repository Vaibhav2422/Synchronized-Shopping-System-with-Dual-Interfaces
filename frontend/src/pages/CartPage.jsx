import { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { Trash2, Tag, X, Users, Star, ShoppingBag, RefreshCw } from 'lucide-react';
import { getCart, removeFromCart, getBill, applyCoupon, removeCoupon, splitBill, getLoyalty } from '../api';
import toast from 'react-hot-toast';

function BillRow({ label, value, color, prefix = '-₹', bold }) {
  if (value === 0 || value === undefined) return null;
  return (
    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center',
                  padding: '6px 0', fontWeight: bold ? 700 : 400 }}>
      <span style={{ color: 'var(--text-dim)', fontSize: '13px' }}>{label}</span>
      <span style={{ color: color || 'var(--text)', fontSize: '14px' }}>
        {prefix}{Math.abs(value).toFixed(2)}
      </span>
    </div>
  );
}

export default function CartPage() {
  const nav = useNavigate();
  const [cart, setCart] = useState(null);
  const [bill, setBill] = useState(null);
  const [loyalty, setLoyalty] = useState(null);
  const [couponInput, setCouponInput] = useState('');
  const [splitCount, setSplitCount] = useState(1);
  const [redeemLoyalty, setRedeemLoyalty] = useState(false);
  const [loading, setLoading] = useState(true);

  const refresh = async () => {
    try {
      const [c, b, l] = await Promise.all([getCart(), getBill(redeemLoyalty), getLoyalty()]);
      setCart(c.data); setBill(b.data); setLoyalty(l.data);
    } catch { toast.error('Failed to load cart'); }
    finally { setLoading(false); }
  };

  useEffect(() => { refresh(); }, [redeemLoyalty]);

  const handleRemove = async (id) => {
    try { await removeFromCart(id); toast.success('Removed'); refresh(); }
    catch (e) { toast.error(e.response?.data?.error || 'Error'); }
  };

  const handleApplyCoupon = async () => {
    if (!couponInput.trim()) return;
    try {
      await applyCoupon(couponInput.trim().toUpperCase());
      toast.success(`Coupon ${couponInput.toUpperCase()} applied!`);
      setCouponInput(''); refresh();
    } catch (e) { toast.error(e.response?.data?.error || 'Invalid coupon'); }
  };

  const handleRemoveCoupon = async (code) => {
    try { await removeCoupon(code); toast.success('Coupon removed'); refresh(); }
    catch (e) { toast.error(e.response?.data?.error || 'Error'); }
  };

  const handleSplit = async (n) => {
    setSplitCount(n);
    try { const r = await splitBill(n); setBill(r.data); }
    catch { toast.error('Split failed'); }
  };

  if (loading) return (
    <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '60vh' }}>
      <span className="spinner" style={{ width: 40, height: 40 }} />
    </div>
  );

  const isEmpty = !cart || cart.itemCount === 0;

  return (
    <div style={{ maxWidth: 1100, margin: '0 auto', padding: '32px 24px' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '28px' }}>
        <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '28px', fontWeight: 700 }}
            className="gold-text">Shopping Cart</h1>
        <button className="btn btn-outline" onClick={refresh} style={{ padding: '8px 12px' }}>
          <RefreshCw size={14} />
        </button>
      </div>

      {isEmpty ? (
        <div style={{ textAlign: 'center', padding: '80px 0', color: 'var(--text-dim)' }}>
          <ShoppingBag size={56} style={{ marginBottom: '16px', opacity: 0.3 }} />
          <p style={{ fontSize: '18px', marginBottom: '16px' }}>Your cart is empty</p>
          <button className="btn btn-gold" onClick={() => nav('/')}>Browse Products</button>
        </div>
      ) : (
        <div style={{ display: 'grid', gridTemplateColumns: '1fr 380px', gap: '24px' }}>
          {/* Cart items */}
          <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
            {cart.items.map(item => (
              <div key={item.product.id} className="glass" style={{ padding: '16px', display: 'flex', gap: '16px', alignItems: 'center' }}>
                <div style={{
                  width: 48, height: 48, borderRadius: '10px',
                  background: 'var(--gold-dim)', border: '1px solid var(--border)',
                  display: 'flex', alignItems: 'center', justifyContent: 'center',
                  fontSize: '20px', flexShrink: 0,
                }}>
                  {item.product.category === 'Electronics' ? '⚡' : item.product.category === 'Clothing' ? '👕' : '📚'}
                </div>
                <div style={{ flex: 1 }}>
                  <div style={{ fontWeight: 600, marginBottom: '4px' }}>{item.product.name}</div>
                  <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>
                    ₹{item.product.price.toFixed(2)} × {item.quantity}
                    {item.lineDiscount > 0 && (
                      <span style={{ color: 'var(--green)', marginLeft: '8px' }}>
                        -₹{item.lineDiscount.toFixed(2)} off
                      </span>
                    )}
                  </div>
                </div>
                <div style={{ textAlign: 'right' }}>
                  <div className="gold-text" style={{ fontWeight: 700, fontSize: '16px' }}>
                    ₹{item.lineTotal.toFixed(2)}
                  </div>
                  <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>qty: {item.quantity}</div>
                </div>
                <button className="btn btn-danger" onClick={() => handleRemove(item.product.id)}
                        style={{ padding: '8px', minWidth: 'unset' }}>
                  <Trash2 size={14} />
                </button>
              </div>
            ))}
          </div>

          {/* Bill panel */}
          <div style={{ display: 'flex', flexDirection: 'column', gap: '16px' }}>
            {/* Coupon */}
            <div className="glass" style={{ padding: '16px' }}>
              <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '12px' }}>
                <Tag size={16} color="var(--gold)" />
                <span style={{ fontWeight: 600, fontSize: '14px' }}>Coupons</span>
                <span className="badge badge-blue" style={{ marginLeft: 'auto', fontSize: '10px' }}>Stack multiple!</span>
              </div>
              <div style={{ display: 'flex', gap: '8px', marginBottom: '10px' }}>
                <input className="input" placeholder="SAVE10 or FESTIVE20"
                       value={couponInput} onChange={e => setCouponInput(e.target.value)}
                       onKeyDown={e => e.key === 'Enter' && handleApplyCoupon()}
                       style={{ fontSize: '13px' }} />
                <button className="btn btn-ghost" onClick={handleApplyCoupon} style={{ whiteSpace: 'nowrap' }}>Apply</button>
              </div>
              {bill?.appliedCoupons?.length > 0 && (
                <div style={{ display: 'flex', flexWrap: 'wrap', gap: '6px' }}>
                  {bill.appliedCoupons.map(c => (
                    <div key={c} style={{
                      display: 'flex', alignItems: 'center', gap: '6px',
                      background: 'rgba(74,222,128,0.08)', border: '1px solid rgba(74,222,128,0.2)',
                      borderRadius: '6px', padding: '4px 10px', fontSize: '12px', color: 'var(--green)',
                    }}>
                      {c}
                      <X size={12} style={{ cursor: 'pointer' }} onClick={() => handleRemoveCoupon(c)} />
                    </div>
                  ))}
                </div>
              )}
            </div>

            {/* Bill split */}
            <div className="glass" style={{ padding: '16px' }}>
              <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '12px' }}>
                <Users size={16} color="var(--gold)" />
                <span style={{ fontWeight: 600, fontSize: '14px' }}>Split Bill</span>
              </div>
              <div style={{ display: 'flex', gap: '8px', alignItems: 'center' }}>
                <input className="input" type="number" min="1" max="20" value={splitCount}
                       onChange={e => handleSplit(parseInt(e.target.value) || 1)}
                       style={{ width: '80px', textAlign: 'center' }} />
                <span style={{ fontSize: '13px', color: 'var(--text-dim)' }}>people</span>
                {splitCount > 1 && bill && (
                  <span className="gold-text" style={{ fontWeight: 700, marginLeft: 'auto' }}>
                    ₹{bill.perPerson?.toFixed(2)} each
                  </span>
                )}
              </div>
            </div>

            {/* Loyalty */}
            {loyalty && loyalty.points > 0 && (
              <div className="glass" style={{ padding: '16px' }}>
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '12px' }}>
                  <Star size={16} color="var(--gold)" />
                  <span style={{ fontWeight: 600, fontSize: '14px' }}>Loyalty Points</span>
                  <span className="badge badge-gold" style={{ marginLeft: 'auto' }}>{loyalty.points} pts</span>
                </div>
                <div style={{ fontSize: '12px', color: 'var(--text-dim)', marginBottom: '10px' }}>
                  Worth ₹{loyalty.rupeeValue?.toFixed(2)} — redeem at checkout
                </div>
                <label style={{ display: 'flex', alignItems: 'center', gap: '8px', cursor: 'pointer', fontSize: '13px' }}>
                  <input type="checkbox" checked={redeemLoyalty} onChange={e => setRedeemLoyalty(e.target.checked)}
                         style={{ accentColor: 'var(--gold)' }} />
                  Redeem points (₹{loyalty.rupeeValue?.toFixed(2)} off)
                </label>
              </div>
            )}

            {/* Bill breakdown */}
            {bill && (
              <div className="glass" style={{ padding: '16px' }}>
                <div style={{ fontWeight: 600, fontSize: '14px', marginBottom: '12px' }}>Bill Breakdown</div>
                <BillRow label="Subtotal" value={bill.subtotal} color="var(--text)" prefix="₹" />
                <BillRow label="Product Discounts" value={bill.productDiscounts} color="var(--green)" />
                <BillRow label="Cart Discount (5%)" value={bill.cartDiscount} color="var(--green)" />
                <BillRow label="Coupon Discounts" value={bill.stackedCouponDiscount} color="var(--green)" />
                <BillRow label="Loyalty Discount" value={bill.loyaltyDiscount} color="var(--green)" />
                <div className="divider" />
                <BillRow label="Taxable Amount" value={bill.taxableAmount} color="var(--text)" prefix="₹" />
                <BillRow label="CGST (9%)" value={bill.cgst} color="var(--text-dim)" prefix="₹" />
                <BillRow label="SGST (9%)" value={bill.sgst} color="var(--text-dim)" prefix="₹" />
                <div className="divider" />
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                  <span style={{ fontWeight: 700, fontSize: '15px' }}>Total</span>
                  <span className="gold-text" style={{ fontWeight: 700, fontSize: '22px' }}>
                    ₹{bill.finalTotal?.toFixed(2)}
                  </span>
                </div>
                {bill.earnedPoints > 0 && (
                  <div style={{ marginTop: '8px', fontSize: '12px', color: 'var(--gold)', textAlign: 'right' }}>
                    +{bill.earnedPoints} loyalty points on purchase
                  </div>
                )}
              </div>
            )}

            <button className="btn btn-gold" onClick={() => nav('/checkout')}
                    style={{ width: '100%', justifyContent: 'center', padding: '14px' }}>
              Proceed to Checkout →
            </button>
          </div>
        </div>
      )}
    </div>
  );
}
