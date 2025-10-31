-- initdb/init.sql
CREATE TABLE IF NOT EXISTS accounts (
    id SERIAL PRIMARY KEY,
    owner VARCHAR(255) NOT NULL,
    amount INTEGER NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS operations (
    id SERIAL PRIMARY KEY,
    account_id INTEGER REFERENCES accounts(id) ON DELETE CASCADE,
    kind VARCHAR(50) NOT NULL,
    amount INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);
