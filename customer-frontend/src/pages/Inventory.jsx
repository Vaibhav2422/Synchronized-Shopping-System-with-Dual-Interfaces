import { useState, useEffect } from 'react';
import { Plus, Pencil, Trash2, Package } from 'lucide-react';
import { getProducts, addProduct, updateProduct, deleteProduct } from '../api';
import toast from 'react-hot-toast';

const EMPTY_FORM = { type: 'Electronics', id: '', name: '', price: '', quantity: '',
                     brand: '', warranty: '', size: '', fabric: '', author: '', genre: '' };

export default function Inventory() {
  const [products, setProducts] = useState([]);
  const [showForm, setShowForm] = useState(false);
  const [form, setForm] = useState(EMPTY_FORM);
  const [editId, setEditId] = useState(null);
  const [loading, setLoading] = useState(false);

  const load = () => getProducts().then(r => setProducts(r.data)).catch(() => {});
  useEffect(() => { load(); }, []);

  const handleAdd = async () => {
    setLoading(true);
    try {
      const payload = {
        ...form,
        id: parseInt(form.id),
        price: parseFloat(form.price),
        quantity: parseInt(form.quantity),
        warranty: parseInt(form.warranty) || 0,
      };
      await addProduct(payload);
      toast.success('Product added!');
      setShowForm(false); setForm(EMPTY_FORM); load();
    } catch (e) { toast.error(e.response?.data?.error || 'Failed to add'); }
    finally { setLoading(false); }
  };

  const handleUpdate = async (id) => {
    try {
      await updateProduct(id, { price: parseFloat(form.price), quantity: parseInt(form.quantity) });
      toast.success('Updated!');
      setEditId(null); setForm(EMPTY_FORM); load();
    } catch (e) { toast.error(e.response?.data?.error || 'Failed to update'); }
  };

  const handleDelete = async (id, name) => {
    if (!window.confirm(`Delete "${name}"?`)) return;
    try { await deleteProduct(id); toast.success('Deleted'); load(); }
    catch (e) { toast.error(e.response?.data?.error || 'Failed to delete'); }
  };

  const startEdit = (p) => {
    setEditId(p.id);
    setForm(f => ({ ...f, price: p.price, quantity: p.quantity }));
  };

  return (
    <div style={{ maxWidth: 1100, margin: '0 auto', padding: '32px 24px' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '28px' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
          <Package size={24} color="var(--gold)" />
          <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '26px', fontWeight: 700 }}
              className="gold-text">Inventory</h1>
        </div>
        <button className="btn btn-gold" onClick={() => { setShowForm(true); setEditId(null); setForm(EMPTY_FORM); }}>
          <Plus size={15} /> Add Product
        </button>
      </div>

      {showForm && (
        <div className="glass" style={{ padding: '20px', marginBottom: '24px' }}>
          <div style={{ fontWeight: 600, marginBottom: '16px' }}>New Product</div>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(180px, 1fr))', gap: '12px' }}>
            <div>
              <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '4px' }}>Type</label>
              <select className="input" value={form.type} onChange={e => setForm(f => ({ ...f, type: e.target.value }))}>
                <option>Electronics</option><option>Clothing</option><option>Books</option>
              </select>
            </div>
            {['id', 'name', 'price', 'quantity'].map(k => (
              <div key={k}>
                <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '4px', textTransform: 'capitalize' }}>{k}</label>
                <input className="input" placeholder={k} value={form[k]}
                  onChange={e => setForm(f => ({ ...f, [k]: e.target.value }))} />
              </div>
            ))}
            {form.type === 'Electronics' && ['brand', 'warranty'].map(k => (
              <div key={k}>
                <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '4px', textTransform: 'capitalize' }}>{k}</label>
                <input className="input" placeholder={k} value={form[k]}
                  onChange={e => setForm(f => ({ ...f, [k]: e.target.value }))} />
              </div>
            ))}
            {form.type === 'Clothing' && ['size', 'fabric'].map(k => (
              <div key={k}>
                <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '4px', textTransform: 'capitalize' }}>{k}</label>
                <input className="input" placeholder={k} value={form[k]}
                  onChange={e => setForm(f => ({ ...f, [k]: e.target.value }))} />
              </div>
            ))}
            {form.type === 'Books' && ['author', 'genre'].map(k => (
              <div key={k}>
                <label style={{ fontSize: '12px', color: 'var(--text-dim)', display: 'block', marginBottom: '4px', textTransform: 'capitalize' }}>{k}</label>
                <input className="input" placeholder={k} value={form[k]}
                  onChange={e => setForm(f => ({ ...f, [k]: e.target.value }))} />
              </div>
            ))}
          </div>
          <div style={{ display: 'flex', gap: '10px', marginTop: '16px' }}>
            <button className="btn btn-gold" onClick={handleAdd} disabled={loading}>
              {loading ? <span className="spinner" /> : <Plus size={14} />} Add
            </button>
            <button className="btn btn-outline" onClick={() => setShowForm(false)}>Cancel</button>
          </div>
        </div>
      )}

      <div className="glass" style={{ overflow: 'hidden' }}>
        <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '13px' }}>
          <thead>
            <tr style={{ borderBottom: '1px solid var(--border)', background: 'rgba(0,0,0,0.2)' }}>
              {['ID', 'Name', 'Category', 'Price', 'Stock', 'Actions'].map(h => (
                <th key={h} style={{ padding: '12px 16px', textAlign: 'left', color: 'var(--text-dim)', fontWeight: 500 }}>{h}</th>
              ))}
            </tr>
          </thead>
          <tbody>
            {products.map(p => (
              <tr key={p.id} style={{ borderBottom: '1px solid rgba(212,175,55,0.05)' }}>
                <td style={{ padding: '12px 16px', color: 'var(--gold)' }}>#{p.id}</td>
                <td style={{ padding: '12px 16px', fontWeight: 500 }}>{p.name}</td>
                <td style={{ padding: '12px 16px' }}><span className="badge badge-gold">{p.category}</span></td>
                <td style={{ padding: '12px 16px' }}>
                  {editId === p.id
                    ? <input className="input" type="number" value={form.price}
                        onChange={e => setForm(f => ({ ...f, price: e.target.value }))}
                        style={{ width: '100px', padding: '6px 10px' }} />
                    : `₹${p.price.toFixed(2)}`}
                </td>
                <td style={{ padding: '12px 16px' }}>
                  {editId === p.id
                    ? <input className="input" type="number" value={form.quantity}
                        onChange={e => setForm(f => ({ ...f, quantity: e.target.value }))}
                        style={{ width: '80px', padding: '6px 10px' }} />
                    : <span style={{ color: p.quantity > 0 ? 'var(--green)' : 'var(--red)' }}>{p.quantity}</span>}
                </td>
                <td style={{ padding: '12px 16px' }}>
                  <div style={{ display: 'flex', gap: '8px' }}>
                    {editId === p.id ? (
                      <>
                        <button className="btn btn-gold" onClick={() => handleUpdate(p.id)} style={{ padding: '6px 12px', fontSize: '12px' }}>Save</button>
                        <button className="btn btn-outline" onClick={() => setEditId(null)} style={{ padding: '6px 12px', fontSize: '12px' }}>Cancel</button>
                      </>
                    ) : (
                      <>
                        <button className="btn btn-ghost" onClick={() => startEdit(p)} style={{ padding: '6px 10px' }}><Pencil size={13} /></button>
                        <button className="btn btn-danger" onClick={() => handleDelete(p.id, p.name)} style={{ padding: '6px 10px' }}><Trash2 size={13} /></button>
                      </>
                    )}
                  </div>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
        {products.length === 0 && (
          <div style={{ textAlign: 'center', padding: '40px', color: 'var(--text-dim)' }}>
            <Package size={32} style={{ marginBottom: '8px', opacity: 0.3 }} />
            <p>No products</p>
          </div>
        )}
      </div>
    </div>
  );
}
