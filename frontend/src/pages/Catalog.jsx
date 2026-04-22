import { useState, useEffect, useCallback } from 'react';
import { Search, Filter, ShoppingCart, Zap, BookOpen, Shirt } from 'lucide-react';
import { getProducts, searchProducts, filterProducts, addToCart } from '../api';
import toast from 'react-hot-toast';

const CATS = ['All', 'Electronics', 'Clothing', 'Books'];
const CAT_ICONS = { Electronics: Zap, Clothing: Shirt, Books: BookOpen };

function ProductCard({ p, onAdd }) {
  const [adding, setAdding] = useState(false);
  const discPct = p.price > 0 ? Math.round((p.discount / p.price) * 100) : 0;
  const discounted = p.price - p.discount;
  const Icon = CAT_ICONS[p.category] || Zap;

  const handleAdd = async () => {
    setAdding(true);
    try {
      await addToCart(p.id, 1);
      toast.success(`${p.name} added to cart`);
      onAdd();
    } catch (e) {
      toast.error(e.response?.data?.error || 'Failed to add');
    } finally { setAdding(false); }
  };

  return (
    <div className="glass fade-in" style={{ padding: '20px', display: 'flex', flexDirection: 'column', gap: '12px', transition: 'border-color 0.2s' }}
         onMouseEnter={e => e.currentTarget.style.borderColor = 'rgba(212,175,55,0.35)'}
         onMouseLeave={e => e.currentTarget.style.borderColor = 'rgba(212,175,55,0.15)'}>
      {/* Header */}
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start' }}>
        <div style={{
          width: 44, height: 44, borderRadius: '10px',
          background: 'rgba(212,175,55,0.08)', border: '1px solid rgba(212,175,55,0.15)',
          display: 'flex', alignItems: 'center', justifyContent: 'center',
        }}>
          <Icon size={20} color="#d4af37" />
        </div>
        <div style={{ display: 'flex', gap: '6px', flexWrap: 'wrap', justifyContent: 'flex-end' }}>
          <span className="badge badge-gold">{p.category}</span>
          {discPct > 0 && <span className="badge badge-green">-{discPct}%</span>}
        </div>
      </div>

      {/* Name */}
      <div>
        <div style={{ fontWeight: 600, fontSize: '15px', marginBottom: '4px' }}>{p.name}</div>
        <div style={{ fontSize: '12px', color: 'var(--text-dim)' }}>ID: #{p.id}</div>
      </div>

      {/* Price */}
      <div style={{ display: 'flex', alignItems: 'baseline', gap: '8px' }}>
        <span className="gold-text" style={{ fontSize: '20px', fontWeight: 700 }}>
          ₹{discounted.toFixed(2)}
        </span>
        {discPct > 0 && (
          <span style={{ fontSize: '13px', color: 'var(--text-dim)', textDecoration: 'line-through' }}>
            ₹{p.price.toFixed(2)}
          </span>
        )}
      </div>

      {/* Stock */}
      <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
        <div style={{
          width: 8, height: 8, borderRadius: '50%',
          background: p.quantity > 0 ? 'var(--green)' : 'var(--red)',
          boxShadow: p.quantity > 0 ? '0 0 6px var(--green)' : 'none',
        }} />
        <span style={{ fontSize: '12px', color: 'var(--text-dim)' }}>
          {p.quantity > 0 ? `${p.quantity} in stock` : 'Out of stock'}
        </span>
      </div>

      {/* Add button */}
      <button className="btn btn-gold" onClick={handleAdd}
              disabled={adding || p.quantity === 0}
              style={{ width: '100%', justifyContent: 'center', marginTop: 'auto' }}>
        {adding ? <span className="spinner" /> : <ShoppingCart size={15} />}
        {adding ? 'Adding...' : 'Add to Cart'}
      </button>
    </div>
  );
}

export default function Catalog() {
  const [products, setProducts] = useState([]);
  const [loading, setLoading] = useState(true);
  const [search, setSearch] = useState('');
  const [cat, setCat] = useState('All');
  const [refresh, setRefresh] = useState(0);

  const load = useCallback(async () => {
    setLoading(true);
    try {
      let res;
      if (search.trim()) res = await searchProducts(search.trim());
      else if (cat !== 'All') res = await filterProducts(cat);
      else res = await getProducts();
      setProducts(res.data);
    } catch { toast.error('Failed to load products'); }
    finally { setLoading(false); }
  }, [search, cat, refresh]);

  useEffect(() => { load(); }, [load]);

  // Debounce search
  useEffect(() => {
    const t = setTimeout(() => load(), 300);
    return () => clearTimeout(t);
  }, [search]);

  return (
    <div style={{ maxWidth: 1200, margin: '0 auto', padding: '32px 24px' }}>
      {/* Header */}
      <div style={{ marginBottom: '32px' }}>
        <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '32px', fontWeight: 700, marginBottom: '8px' }}
            className="gold-text">Product Catalog</h1>
        <p style={{ color: 'var(--text-dim)', fontSize: '14px' }}>
          {products.length} products available
        </p>
      </div>

      {/* Search + Filter */}
      <div style={{ display: 'flex', gap: '12px', marginBottom: '28px', flexWrap: 'wrap' }}>
        <div style={{ position: 'relative', flex: 1, minWidth: '200px' }}>
          <Search size={16} style={{ position: 'absolute', left: 12, top: '50%', transform: 'translateY(-50%)', color: 'var(--text-dim)' }} />
          <input className="input" placeholder="Search products..."
                 value={search} onChange={e => setSearch(e.target.value)}
                 style={{ paddingLeft: '38px' }} />
        </div>
        <div style={{ display: 'flex', gap: '8px', flexWrap: 'wrap' }}>
          {CATS.map(c => (
            <button key={c} className={`btn ${cat === c ? 'btn-ghost' : 'btn-outline'}`}
                    onClick={() => { setCat(c); setSearch(''); }}
                    style={{ fontSize: '13px', padding: '8px 16px' }}>
              {c === 'All' ? <Filter size={14} /> : null}
              {c}
            </button>
          ))}
        </div>
      </div>

      {/* Grid */}
      {loading ? (
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(240px, 1fr))', gap: '16px' }}>
          {[...Array(6)].map((_, i) => (
            <div key={i} className="skeleton" style={{ height: '260px' }} />
          ))}
        </div>
      ) : products.length === 0 ? (
        <div style={{ textAlign: 'center', padding: '80px 0', color: 'var(--text-dim)' }}>
          <Search size={48} style={{ marginBottom: '16px', opacity: 0.3 }} />
          <p>No products found</p>
        </div>
      ) : (
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(240px, 1fr))', gap: '16px' }}>
          {products.map(p => (
            <ProductCard key={p.id} p={p} onAdd={() => setRefresh(r => r + 1)} />
          ))}
        </div>
      )}
    </div>
  );
}
