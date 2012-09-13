#include <xen/xen.h>
#include <xen/grant_table.h>
#include <xen/hvm.h>
#include <xen/interface/xen.h>
#include <xen/interface/memory.h>
#include <xen/interface/hvm/params.h>
#include <xen/features.h>
#include <xen/platform_pci.h>
#include <xen/xenbus.h>
#include <asm/xen/hypervisor.h>
#include <asm/xen/hypercall.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

struct start_info _xen_start_info;
struct start_info *xen_start_info = &_xen_start_info;
EXPORT_SYMBOL_GPL(xen_start_info);

enum xen_domain_type xen_domain_type = XEN_NATIVE;
EXPORT_SYMBOL_GPL(xen_domain_type);

struct shared_info xen_dummy_shared_info;
struct shared_info *HYPERVISOR_shared_info = (void *)&xen_dummy_shared_info;

DEFINE_PER_CPU(struct vcpu_info *, xen_vcpu);

/* TODO: to be removed */
__read_mostly int xen_have_vector_callback;
EXPORT_SYMBOL_GPL(xen_have_vector_callback);

int xen_platform_pci_unplug = XEN_UNPLUG_ALL;
EXPORT_SYMBOL_GPL(xen_platform_pci_unplug);

int xen_remap_domain_mfn_range(struct vm_area_struct *vma,
			       unsigned long addr,
			       unsigned long mfn, int nr,
			       pgprot_t prot, unsigned domid)
{
	return -ENOSYS;
}
EXPORT_SYMBOL_GPL(xen_remap_domain_mfn_range);

/*
 * see Documentation/devicetree/bindings/arm/xen.txt for the
 * documentation of the Xen Device Tree format.
 */
#define GRANT_TABLE_PHYSADDR 0
static int __init xen_guest_init(void)
{
	struct xen_add_to_physmap xatp;
	static struct shared_info *shared_info_page = 0;
	struct device_node *node;
	int len;
	const char *s = NULL;
	const char *version = NULL;
	const char *xen_prefix = "xen,xen-";
	struct resource res;

	node = of_find_compatible_node(NULL, NULL, "xen,xen");
	if (!node) {
		pr_debug("No Xen support\n");
		return 0;
	}
	s = of_get_property(node, "compatible", &len);
	if (strlen(xen_prefix) + 3  < len &&
			!strncmp(xen_prefix, s, strlen(xen_prefix)))
		version = s + strlen(xen_prefix);
	if (version == NULL) {
		pr_debug("Xen version not found\n");
		return 0;
	}
	if (of_address_to_resource(node, GRANT_TABLE_PHYSADDR, &res))
		return 0;
	xen_hvm_resume_frames = res.start >> PAGE_SHIFT;
	xen_domain_type = XEN_HVM_DOMAIN;

	xen_setup_features();
	if (xen_feature(XENFEAT_dom0))
		xen_start_info->flags |= SIF_INITDOMAIN|SIF_PRIVILEGED;
	else
		xen_start_info->flags &= ~(SIF_INITDOMAIN|SIF_PRIVILEGED);

	if (!shared_info_page)
		shared_info_page = (struct shared_info *)
			get_zeroed_page(GFP_KERNEL);
	if (!shared_info_page) {
		pr_err("not enough memory\n");
		return -ENOMEM;
	}
	xatp.domid = DOMID_SELF;
	xatp.idx = 0;
	xatp.space = XENMAPSPACE_shared_info;
	xatp.gpfn = __pa(shared_info_page) >> PAGE_SHIFT;
	if (HYPERVISOR_memory_op(XENMEM_add_to_physmap, &xatp))
		BUG();

	HYPERVISOR_shared_info = (struct shared_info *)shared_info_page;

	/* xen_vcpu is a pointer to the vcpu_info struct in the shared_info
	 * page, we use it in the event channel upcall and in some pvclock
	 * related functions. We don't need the vcpu_info placement
	 * optimizations because we don't use any pv_mmu or pv_irq op on
	 * HVM.
	 * The shared info contains exactly 1 CPU (the boot CPU). The guest
	 * is required to use VCPUOP_register_vcpu_info to place vcpu info
	 * for secondary CPUs as they are brought up. */
	per_cpu(xen_vcpu, 0) = &HYPERVISOR_shared_info->vcpu_info[0];

	gnttab_init();
	if (!xen_initial_domain())
		xenbus_probe(NULL);

	return 0;
}
core_initcall(xen_guest_init);
