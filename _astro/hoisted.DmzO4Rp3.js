const __vite__mapDeps=(i,m=__vite__mapDeps,d=(m.f||(m.f=["_astro/ui-core.BfafCbc_.js","_astro/Tabs.astro_astro_type_script_index_0_lang.3nBd5krW.js"])))=>i.map(i=>d[i]);
import"./Tabs.astro_astro_type_script_index_0_lang.3nBd5krW.js";const y=document.getElementById("starlight__sidebar"),b=y?.querySelector("sl-sidebar-state-persist"),S="sl-sidebar-state",v=()=>{let o=[];const e=b?.dataset.hash||"";try{const n=sessionStorage.getItem(S),t=JSON.parse(n||"{}");Array.isArray(t.open)&&t.hash===e&&(o=t.open)}catch{}return{hash:e,open:o,scroll:y?.scrollTop||0}},w=o=>{try{sessionStorage.setItem(S,JSON.stringify(o))}catch{}},L=()=>w(v()),I=(o,e)=>{const n=v();n.open[e]=o,w(n)};b?.addEventListener("click",o=>{if(!(o.target instanceof Element))return;const e=o.target.closest("summary")?.closest("details");if(!e)return;const n=e.querySelector("sl-sidebar-restore"),t=parseInt(n?.dataset.index||"");isNaN(t)||I(!e.open,t)});addEventListener("visibilitychange",()=>{document.visibilityState==="hidden"&&L()});addEventListener("pageHide",L);const H="starlight-image-zoom-zoomable";customElements.define("starlight-image-zoom",class T extends HTMLElement{#e;#i=[];#s;#o=this.querySelector("template");#t={image:"starlight-image-zoom-image",opened:"starlight-image-zoom-opened",source:"starlight-image-zoom-source"};static#u=!1;constructor(){super();const e=()=>{(window.requestIdleCallback??(t=>setTimeout(t,1)))(()=>{const t=[...document.querySelectorAll(H)];t.length!==0&&(this.#h(t),document.addEventListener("click",this.#r),window.addEventListener("resize",this.#a))})};window.addEventListener("DOMContentLoaded",e,{once:!0}),document.addEventListener("astro:after-preparation",()=>{document.removeEventListener("click",this.#r),window.removeEventListener("resize",this.#a)},{once:!0}),T.#u||=document.addEventListener("astro:after-swap",e)===void 0}#h(e){for(const n of e){const t=n.querySelector("img");t&&(this.#i.push(t),n.querySelector("button")?.addEventListener("click",s=>{s.stopPropagation(),this.#l(t)}))}}#r=({target:e})=>{if(!(e instanceof HTMLElement&&e.closest("figcaption"))){if(this.#e){this.#n();return}e instanceof HTMLImageElement&&this.#i.includes(e)&&this.#l(e)}};#a=()=>{this.#n(!0)};#c=()=>{this.#n()};#m=e=>{e.preventDefault(),this.#n()};#l(e){if(!this.#o||this.#e)return;this.#s=document.activeElement;const n=document.createElement("div");n.append(this.#o.content.cloneNode(!0));const t=n.querySelector("dialog"),s=t?.querySelector("figure");if(!t||!s)return;const r={overflow:document.body.style.overflow,width:document.body.style.width},i=document.body.clientWidth;document.body.style.overflow="hidden",document.body.style.width=`${i}px`,document.querySelector("header")?.style.setProperty("padding-inline-end",`calc(var(--sl-nav-pad-x) + ${window.innerWidth-i}px)`);const c=this.#f(e);e.classList.add(this.#t.source),c.classList.add(this.#t.image),s.append(c),this.#g(e.getAttribute("alt"),s),document.body.append(n),document.addEventListener("wheel",this.#c,{once:!0}),t.addEventListener("cancel",this.#m),t.showModal(),c.style.transform=this.#p(e,s),document.body.classList.add(this.#t.opened),this.#e={body:r,dialog:t,image:e,zoomedImage:c}}#n(e=!1){if(window.removeEventListener("wheel",this.#c),!this.#e)return;const{zoomedImage:n}=this.#e;n.style.transform="",document.body.classList.remove(this.#t.opened);const{matches:t}=window.matchMedia("(prefers-reduced-motion: reduce)");e||t?this.#d():n.addEventListener("transitionend",this.#d,{once:!0})}#d=()=>{if(!this.#e)return;const{dialog:e,image:n}=this.#e;n.classList.remove(this.#t.source),e.parentElement?.remove(),document.body.style.overflow=this.#e.body.overflow,document.body.style.width=this.#e.body.width,document.querySelector("header")?.style.setProperty("padding-inline-end","var(--sl-nav-pad-x)"),this.#e=void 0,this.#s instanceof HTMLElement&&this.#s.focus()};#g(e,n){if(Object.hasOwn(this.dataset,"hideCaption")||(e=e?.trim()??"",e.length===0))return;const t=document.createElement("figcaption");t.ariaHidden="true",t.textContent=e,n.append(t)}#f(e){const{height:n,left:t,top:s,width:r}=e.getBoundingClientRect(),i=e.cloneNode(!0);return i.removeAttribute("id"),i.style.position="absolute",i.style.width=`${r}px`,i.style.height=`${n}px`,i.style.top=`${s}px`,i.style.left=`${t}px`,i.style.transform="",e.parentElement?.tagName==="PICTURE"&&e.currentSrc&&(i.src=e.currentSrc),i}#p(e,n){const t=this.#E(e),s=e.getBoundingClientRect(),r=n.getBoundingClientRect(),i=t?r.height:e.naturalHeight,c=t?r.width:e.naturalWidth,u=Math.min(Math.max(s.width,c),r.width)/s.width,a=Math.min(Math.max(s.height,i),r.height)/s.height,h=Math.min(u,a),d=(-s.left+(r.width-s.width)/2+r.left)/h,l=(-s.top+(r.height-s.height)/2+r.top)/h;return`scale(${h}) translate3d(${d}px, ${l}px, 0)`}#E(e){return e.currentSrc.toLowerCase().endsWith(".svg")}});class A extends HTMLElement{constructor(){super();const e=this.querySelector("select");e&&e.addEventListener("change",n=>{n.currentTarget instanceof HTMLSelectElement&&(window.location.pathname=n.currentTarget.value)})}}customElements.define("starlight-lang-select",A);const C="modulepreload",_=function(o){return"/"+o},E={},P=function(e,n,t){let s=Promise.resolve();if(n&&n.length>0){document.getElementsByTagName("link");const i=document.querySelector("meta[property=csp-nonce]"),c=i?.nonce||i?.getAttribute("nonce");s=Promise.allSettled(n.map(u=>{if(u=_(u),u in E)return;E[u]=!0;const a=u.endsWith(".css"),h=a?'[rel="stylesheet"]':"";if(document.querySelector(`link[href="${u}"]${h}`))return;const d=document.createElement("link");if(d.rel=a?"stylesheet":C,a||(d.as="script"),d.crossOrigin="",d.href=u,c&&d.setAttribute("nonce",c),document.head.appendChild(d),a)return new Promise((l,m)=>{d.addEventListener("load",l),d.addEventListener("error",()=>m(new Error(`Unable to preload CSS for ${u}`)))})}))}function r(i){const c=new Event("vite:preloadError",{cancelable:!0});if(c.payload=i,window.dispatchEvent(c),!c.defaultPrevented)throw i}return s.then(i=>{for(const c of i||[])c.status==="rejected"&&r(c.reason);return e().catch(r)})};class R extends HTMLElement{constructor(){super();const e=this.querySelector("button[data-open-modal]"),n=this.querySelector("button[data-close-modal]"),t=this.querySelector("dialog"),s=this.querySelector(".dialog-frame"),r=l=>{("href"in(l.target||{})||document.body.contains(l.target)&&!s.contains(l.target))&&c()},i=l=>{t.showModal(),document.body.toggleAttribute("data-search-modal-open",!0),this.querySelector("input")?.focus(),l?.stopPropagation(),window.addEventListener("click",r)},c=()=>t.close();e.addEventListener("click",i),e.disabled=!1,n.addEventListener("click",c),t.addEventListener("close",()=>{document.body.toggleAttribute("data-search-modal-open",!1),window.removeEventListener("click",r)}),window.addEventListener("keydown",l=>{(l.metaKey===!0||l.ctrlKey===!0)&&l.key==="k"&&(t.open?c():i(),l.preventDefault())});let u={};try{u=JSON.parse(this.dataset.translations||"{}")}catch{}const d=this.dataset.stripTrailingSlash!==void 0?l=>l.replace(/(.)\/(#.*)?$/,"$1$2"):l=>l;window.addEventListener("DOMContentLoaded",()=>{(window.requestIdleCallback||(m=>setTimeout(m,1)))(async()=>{const{PagefindUI:m}=await P(async()=>{const{PagefindUI:g}=await import("./ui-core.BfafCbc_.js");return{PagefindUI:g}},__vite__mapDeps([0,1]));new m({element:"#starlight__search",baseUrl:"/",bundlePath:"/".replace(/\/$/,"")+"/pagefind/",showImages:!1,translations:u,showSubResults:!0,processResult:g=>{g.url=d(g.url),g.sub_results=g.sub_results.map(f=>(f.url=d(f.url),f))}})})})}}customElements.define("site-search",R);const M="starlight-theme",x=o=>o==="auto"||o==="dark"||o==="light"?o:"auto",k=()=>x(typeof localStorage<"u"&&localStorage.getItem(M));function O(o){typeof localStorage<"u"&&localStorage.setItem(M,o==="light"||o==="dark"?o:"")}const z=()=>matchMedia("(prefers-color-scheme: light)").matches?"light":"dark";function p(o){StarlightThemeProvider.updatePickers(o),document.documentElement.dataset.theme=o==="auto"?z():o,O(o)}matchMedia("(prefers-color-scheme: light)").addEventListener("change",()=>{k()==="auto"&&p("auto")});class $ extends HTMLElement{constructor(){super(),p(k()),this.querySelector("select")?.addEventListener("change",e=>{e.currentTarget instanceof HTMLSelectElement&&p(x(e.currentTarget.value))})}}customElements.define("starlight-theme-select",$);class N extends HTMLElement{constructor(){super(),this.btn=this.querySelector("button"),this.btn.addEventListener("click",()=>this.toggleExpanded());const e=this.closest("nav");e&&e.addEventListener("keyup",n=>this.closeOnEscape(n))}setExpanded(e){this.setAttribute("aria-expanded",String(e)),document.body.toggleAttribute("data-mobile-menu-expanded",e)}toggleExpanded(){this.setExpanded(this.getAttribute("aria-expanded")!=="true")}closeOnEscape(e){e.code==="Escape"&&(this.setExpanded(!1),this.btn.focus())}}customElements.define("starlight-menu-button",N);const B="_top";class q extends HTMLElement{constructor(){super(),this._current=this.querySelector('a[aria-current="true"]'),this.minH=parseInt(this.dataset.minH||"2",10),this.maxH=parseInt(this.dataset.maxH||"3",10),this.onIdle=e=>(window.requestIdleCallback||(n=>setTimeout(n,1)))(e),this.init=()=>{const e=[...this.querySelectorAll("a")],n=a=>{if(a instanceof HTMLHeadingElement){if(a.id===B)return!0;const h=a.tagName[1];if(h){const d=parseInt(h,10);if(d>=this.minH&&d<=this.maxH)return!0}}return!1},t=a=>{if(!a)return null;const h=a;for(;a;){if(n(a))return a;for(a=a.previousElementSibling;a?.lastElementChild;)a=a.lastElementChild;const d=t(a);if(d)return d}return t(h.parentElement)},s=a=>{for(const{isIntersecting:h,target:d}of a){if(!h)continue;const l=t(d);if(!l)continue;const m=e.find(g=>g.hash==="#"+encodeURIComponent(l.id));if(m){this.current=m;break}}},r=document.querySelectorAll("main [id], main [id] ~ *, main .content > *");let i;const c=()=>{i||(i=new IntersectionObserver(s,{rootMargin:this.getRootMargin()}),r.forEach(a=>i.observe(a)))};c();let u;window.addEventListener("resize",()=>{i&&i.disconnect(),clearTimeout(u),u=setTimeout(()=>this.onIdle(c),200)})},this.onIdle(()=>this.init())}set current(e){e!==this._current&&(this._current&&this._current.removeAttribute("aria-current"),e.setAttribute("aria-current","true"),this._current=e)}getRootMargin(){const e=document.querySelector("header")?.getBoundingClientRect().height||0,n=this.querySelector("summary")?.getBoundingClientRect().height||0,t=e+n+32,s=t+53,r=document.documentElement.clientHeight;return`-${t}px 0% ${s-r}px`}}customElements.define("starlight-toc",q);class D extends q{set current(e){super.current=e;const n=this.querySelector(".display-current");n&&(n.textContent=e.textContent)}constructor(){super();const e=this.querySelector("details");if(!e)return;const n=()=>{e.open=!1};e.querySelectorAll("a").forEach(t=>{t.addEventListener("click",n)}),window.addEventListener("click",t=>{e.contains(t.target)||n()}),window.addEventListener("keydown",t=>{if(t.key==="Escape"&&e.open){const s=e.contains(document.activeElement);if(n(),s){const r=e.querySelector("summary");r&&r.focus()}}})}}customElements.define("mobile-starlight-toc",D);class U extends HTMLElement{constructor(){super();const e=this.querySelector("select");e&&e.addEventListener("change",n=>{n.currentTarget instanceof HTMLSelectElement&&this.querySelectorAll("[data-starlight-multi-sidebar-label]").forEach(s=>{s instanceof HTMLDivElement&&(s.dataset.starlightMultiSidebarLabel===e.value?s.hidden=!1:s.hidden=!0)})})}}customElements.define("starlight-multi-sidebar-select",U);class W extends HTMLElement{constructor(){super();const e=this.querySelector('[role="tablist"]');this.tabs=[...e.querySelectorAll('[role="tab"]')],this.panels=[...this.querySelectorAll(':scope > [role="tabpanel"]')],this.tabs.forEach((n,t)=>{n.addEventListener("click",s=>{s.preventDefault();const r=e.querySelector('[aria-selected="true"]');s.currentTarget!==r&&this.switchTab(s.currentTarget,t)}),n.addEventListener("keydown",s=>{const r=this.tabs.indexOf(s.currentTarget),i=s.key==="ArrowLeft"?r-1:s.key==="ArrowRight"?r+1:s.key==="Home"?0:s.key==="End"?this.tabs.length-1:null;i!==null&&this.tabs[i]&&(s.preventDefault(),this.switchTab(this.tabs[i],i))})})}switchTab(e,n){if(!e)return;this.tabs.forEach(s=>{s.setAttribute("aria-selected","false"),s.setAttribute("tabindex","-1")}),this.panels.forEach(s=>{s.hidden=!0});const t=this.panels[n];t&&(t.hidden=!1),e.removeAttribute("tabindex"),e.setAttribute("aria-selected","true")}}customElements.define("starlight-multi-sidebar-tabs",W);export{P as _};