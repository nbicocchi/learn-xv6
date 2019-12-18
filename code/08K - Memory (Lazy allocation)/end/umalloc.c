#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

#define NALLOC 1024  // Number of block sizes to allocate on call to sbrk
#ifdef NULL
#undef NULL
#endif
#define NULL 0

// long is chosen as an instance of the most restrictive alignment type
typedef long Align;

/* Construct Header data structure.  To ensure that the storage returned by
 * malloc is aligned properly for the objects that are stored in it, all
 * blocks are multiples of the header size, and the header itself is aligned
 * properly.  This is achieved through the use of a union; this data type is big
 * enough to hold the "widest" member, and the alignment is appropriate for all
 * of the types in the union.  Thus by including a member of type Align, which
 * is an instance of the most restrictive type, we guarantee that the size of
 * Header is aligned to the worst-case boundary.  The Align field is never used;
 * it just forces each header to the desired alignment.
 */
union header {
  struct {
    union header *next;
    unsigned size;
  } s;

  Align x;
};
typedef union header Header;


static Header base;           // Used to get an initial member for free list
static Header *freep = NULL;  // Free list starting point


static Header *morecore(unsigned nblocks);
void kandr_free(void *ptr);




void *malloc(unsigned nbytes) {

  Header *currp;
  Header *prevp;
  unsigned nunits;

  /* Calculate the number of memory units needed to provide at least nbytes of
   * memory.
   *
   * Suppose that we need n >= 0 bytes and that the memory unit sizes are b > 0
   * bytes.  Then n / b (using integer division) yields one less than the number
   * of units needed to provide n bytes of memory, except in the case that n is
   * a multiple of b; then it provides exactly the number of units needed.  It
   * can be verified that (n - 1) / b provides one less than the number of units
   * needed to provide n bytes of memory for all values of n > 0.  Thus ((n - 1)
   * / b) + 1 provides exactly the number of units needed for n > 0.
   *
   * The extra sizeof(Header) in the numerator is to include the unit of memory
   * needed for the header itself.
   */
  nunits = ((nbytes + sizeof(Header) - 1) / sizeof(Header)) + 1;

  // case: no free list yet exists; we have to initialize.
  if (freep == NULL) {

    // Create degenerate free list; base points to itself and has size 0
    base.s.next = &base;
    base.s.size = 0;

    // Set free list starting point to base address
    freep = &base;
  }

  /* Initialize pointers to two consecutive blocks in the free list, which we
   * call prevp (the previous block) and currp (the current block)
   */
  prevp = freep;
  currp = prevp->s.next;

  /* Step through the free list looking for a block of memory large enough to
   * fit nunits units of memory into.  If the whole list is traversed without
   * finding such a block, then morecore is called to request more memory from
   * the OS.
   */
  for (; ; prevp = currp, currp = currp->s.next) {

    /* case: found a block of memory in free list large enough to fit nunits
     * units of memory into.  Partition block if necessary, remove it from the
     * free list, and return the address of the block (after moving past the
     * header).
     */
    if (currp->s.size >= nunits) {

      /* case: block is exactly the right size; remove the block from the free
       * list by pointing the previous block to the next block.
       */
      if (currp->s.size == nunits) {
    /* Note that this line wouldn't work as intended if we were down to only
     * 1 block.  However, we would never make it here in that scenario
     * because the block at &base has size 0 and thus the conditional will
     * fail (note that nunits is always >= 1).  It is true that if the block
     * at &base had combined with another block, then previous statement
     * wouldn't apply - but presumably since base is a global variable and
     * future blocks are allocated on the heap, we can be sure that they
     * won't border each other.
     */
    prevp->s.next = currp->s.next;
      }
      /* case: block is larger than the amount of memory asked for; allocate
       * tail end of the block to the user.
       */
      else {
    // Changes the memory stored at currp to reflect the reduced block size
    currp->s.size -= nunits;
    // Find location at which to create the block header for the new block
    currp += currp->s.size;
    // Store the block size in the new header
    currp->s.size = nunits;
      }

      /* Set global starting position to the previous pointer.  Next call to
       * malloc will start either at the remaining part of the partitioned block
       * if a partition occurred, or at the block after the selected block if
       * not.
       */
      freep = prevp;

      /* Return the location of the start of the memory, i.e. after adding one
       * so as to move past the header
       */
      return (void *) (currp + 1);

    } // end found a block of memory in free list case

    /* case: we've wrapped around the free list without finding a block large
     * enough to fit nunits units of memory into.  Call morecore to request that
     * at least nunits units of memory are allocated.
     */
    if (currp == freep) {
      /* morecore returns freep; the reason that we have to assign currp to it
       * again (since we just tested that they are equal), is that there is a
       * call to free inside of morecore that can potentially change the value
       * of freep.  Thus we reassign it so that we can be assured that the newly
       * added block is found before (currp == freep) again.
       */
      if ((currp = morecore(nunits)) == NULL) {
    return NULL;
      }
    } // end wrapped around free list case
  } // end step through free list looking for memory loop
}




static Header *morecore(unsigned nunits) {

  void *freemem;    // The address of the newly created memory
  Header *insertp;  // Header ptr for integer arithmatic and constructing header

  /* Obtaining memory from OS is a comparatively expensive operation, so obtain
   * at least NALLOC blocks of memory and partition as needed
   */
  if (nunits < NALLOC) {
    nunits = NALLOC;
  }

  /* Request that the OS increment the program's data space.  sbrk changes the
   * location of the program break, which defines the end of the process's data
   * segment (i.e., the program break is the first location after the end of the
   * uninitialized data segment).  Increasing the program break has the effect
   * of allocating memory to the process.  On success, brk returns the previous
   * break - so if the break was increased, then this value is a pointer to the
   * start of the newly allocated memory.
   */
  freemem = sbrk(nunits * sizeof(Header));
  // case: unable to allocate more memory; sbrk returns (void *) -1 on error
  if (freemem == (void *) -1) {
    return NULL;
  }

  // Construct new block
  insertp = (Header *) freemem;
  insertp->s.size = nunits;

  /* Insert block into the free list so that it is available for malloc.  Note
   * that we add 1 to the address, effectively moving to the first position
   * after the header data, since of course we want the block header to be
   * transparent for the user's interactions with malloc and free.
   */
  free((void *) (insertp + 1));

  /* Returns the start of the free list; recall that freep has been set to the
   * block immediately preceeding the newly allocated memory (by free).  Thus by
   * returning this value the calling function can immediately find the new
   * memory by following the pointer to the next block.
   */
  return freep;
}




void free(void *ptr) {

  Header *insertp, *currp;

  // Find address of block header for the data to be inserted
  insertp = ((Header *) ptr) - 1;

  /* Step through the free list looking for the position in the list to place
   * the insertion block.  In the typical circumstances this would be the block
   * immediately to the left of the insertion block; this is checked for by
   * finding a block that is to the left of the insertion block and such that
   * the following block in the list is to the right of the insertion block.
   * However this check doesn't check for one such case, and misses another.  We
   * still have to check for the cases where either the insertion block is
   * either to the left of every other block owned by malloc (the case that is
   * missed), or to the right of every block owned by malloc (the case not
   * checked for).  These last two cases are what is checked for by the
   * condition inside of the body of the loop.
   */
  for (currp = freep; !((currp < insertp) && (insertp < currp->s.next)); currp = currp->s.next) {

    /* currp >= currp->s.ptr implies that the current block is the rightmost
     * block in the free list.  Then if the insertion block is to the right of
     * that block, then it is the new rightmost block; conversely if it is to
     * the left of the block that currp points to (which is the current leftmost
     * block), then the insertion block is the new leftmost block.  Note that
     * this conditional handles the case where we only have 1 block in the free
     * list (this case is the reason that we need >= in the first test rather
     * than just >).
     */
    if ((currp >= currp->s.next) && ((currp < insertp) || (insertp < currp->s.next))) {
      break;
    }
  }

  /* Having found the correct location in the free list to place the insertion
   * block, now we have to (i) link it to the next block, and (ii) link the
   * previous block to it.  These are the tasks of the next two if/else pairs.
   */

  /* case: the end of the insertion block is adjacent to the beginning of
   * another block of data owned by malloc.  Absorb the block on the right into
   * the block on the left (i.e. the previously existing block is absorbed into
   * the insertion block).
   */
  if ((insertp + insertp->s.size) == currp->s.next) {
    insertp->s.size += currp->s.next->s.size;
    insertp->s.next = currp->s.next->s.next;
  }
  /* case: the insertion block is not left-adjacent to the beginning of another
   * block of data owned by malloc.  Set the insertion block member to point to
   * the next block in the list.
   */
  else {
    insertp->s.next = currp->s.next;
  }

  /* case: the end of another block of data owned by malloc is adjacent to the
   * beginning of the insertion block.  Absorb the block on the right into the
   * block on the left (i.e. the insertion block is absorbed into the preceeding
   * block).
   */
  if ((currp + currp->s.size) == insertp) {
    currp->s.size += insertp->s.size;
    currp->s.next = insertp->s.next;
  }
  /* case: the insertion block is not right-adjacent to the end of another block
   * of data owned by malloc.  Set the previous block in the list to point to
   * the insertion block.
   */
  else {
    currp->s.next = insertp;
  }

  /* Set the free pointer list to start the block previous to the insertion
   * block.  This makes sense because calls to malloc start their search for
   * memory at the next block after freep, and the insertion block has as good a
   * chance as any of containing a reasonable amount of memory since we've just
   * added some to it.  It also coincides with calls to morecore from
   * kandr_malloc because the next search in the iteration looks at exactly the
   * right memory block.
   */
  freep = currp;
}
