from requests_html import HTMLSession

session = HTMLSession()

url = 'https://api.blockcypher.com/v1/btc/test3/txs/c298fcc5eb1c39743426d86e3c11770a4b261189e0274ef88cdef732bd343bfc'

r = session.get(url)

print(r.html.text)
