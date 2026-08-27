export function Card({children,className=""}:{children:React.ReactNode,className?:string}){return <div className={`rounded-2xl border border-zinc-800 bg-zinc-950 p-5 shadow-xl ${className}`}>{children}</div>}
export function Badge({children}:{children:React.ReactNode}){return <span className="rounded-full bg-zinc-800 px-2.5 py-1 text-xs text-zinc-300">{children}</span>}
export function Metric({label,value}:{label:string,value:string}){return <Card><p className="text-sm text-zinc-400">{label}</p><p className="mt-2 text-2xl font-semibold">{value}</p></Card>}
