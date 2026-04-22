import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { LogIn, Shield, Lock, User } from 'lucide-react';
import toast from 'react-hot-toast';
import { adminLogin } from '../api';
import { useAuth } from '../AuthContext';

export default function Login() {
  const navigate = useNavigate();
  const { signIn } = useAuth();
  const [form, setForm] = useState({ username: '', password: '' });
  const [loading, setLoading] = useState(false);

  const handleSubmit = async (e) => {
    e.preventDefault();
    if (!form.username.trim() || !form.password.trim()) {
      toast.error('Enter username and password');
      return;
    }
    setLoading(true);
    try {
      await adminLogin(form.username.trim(), form.password);
      signIn();
      toast.success('Welcome, Admin!');
      navigate('/analytics');
    } catch {
      toast.error('Invalid credentials');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{
      minHeight: '100vh', display: 'flex', alignItems: 'center', justifyContent: 'center',
      padding: '24px',
      background: 'radial-gradient(ellipse at 50% 0%, rgba(212,175,55,0.06) 0%, transparent 60%)',
    }}>
      <div className="glass-lg fade-in" style={{ width: '100%', maxWidth: '420px', padding: '40px' }}>
        <div style={{ textAlign: 'center', marginBottom: '32px' }}>
          <div style={{
            width: 56, height: 56, borderRadius: '16px',
            background: 'linear-gradient(135deg, #d4af37, #b8960a)',
            display: 'flex', alignItems: 'center', justifyContent: 'center',
            margin: '0 auto 16px',
          }}>
            <Shield size={28} color="#0a0a0f" />
          </div>
          <h1 style={{ fontFamily: 'Playfair Display, serif', fontSize: '26px', fontWeight: 700 }}
              className="gold-text">Admin Panel</h1>
          <p style={{ color: 'var(--text-dim)', fontSize: '14px', marginTop: '6px' }}>Restricted access</p>
        </div>

        <form onSubmit={handleSubmit} style={{ display: 'flex', flexDirection: 'column', gap: '16px' }}>
          <div>
            <label style={{ display: 'block', fontSize: '12px', fontWeight: 600, color: 'var(--text-dim)', marginBottom: '6px', textTransform: 'uppercase', letterSpacing: '0.5px' }}>
              Username
            </label>
            <div style={{ position: 'relative' }}>
              <User size={15} style={{ position: 'absolute', left: 12, top: '50%', transform: 'translateY(-50%)', color: 'var(--text-dim)' }} />
              <input className="input" style={{ paddingLeft: '36px' }} placeholder="admin"
                value={form.username} onChange={e => setForm(f => ({ ...f, username: e.target.value }))} autoFocus />
            </div>
          </div>

          <div>
            <label style={{ display: 'block', fontSize: '12px', fontWeight: 600, color: 'var(--text-dim)', marginBottom: '6px', textTransform: 'uppercase', letterSpacing: '0.5px' }}>
              Password
            </label>
            <div style={{ position: 'relative' }}>
              <Lock size={15} style={{ position: 'absolute', left: 12, top: '50%', transform: 'translateY(-50%)', color: 'var(--text-dim)' }} />
              <input className="input" style={{ paddingLeft: '36px' }} type="password" placeholder="••••••••"
                value={form.password} onChange={e => setForm(f => ({ ...f, password: e.target.value }))} />
            </div>
          </div>

          <button type="submit" className="btn btn-gold"
            style={{ width: '100%', justifyContent: 'center', padding: '12px', marginTop: '8px', fontSize: '15px' }}
            disabled={loading}>
            {loading ? <span className="spinner" /> : <LogIn size={16} />}
            {loading ? 'Signing in…' : 'Sign In'}
          </button>
        </form>

        <p style={{ textAlign: 'center', fontSize: '12px', color: 'var(--text-dim)', marginTop: '24px' }}>
          Default: admin / admin123
        </p>
      </div>
    </div>
  );
}
