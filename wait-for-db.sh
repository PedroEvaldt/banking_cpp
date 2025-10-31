#!/bin/sh
# Aguarda o banco ficar disponível antes de rodar o app
echo "⏳ Aguardando PostgreSQL iniciar..."
until pg_isready -h postgres-bank -p 5432 -U postgres > /dev/null 2>&1; do
  sleep 1
done
echo "✅ Banco está pronto! Iniciando aplicação..."
exec "$@"
